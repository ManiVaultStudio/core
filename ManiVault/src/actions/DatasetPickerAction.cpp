// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetPickerAction.h"

#include "event/Event.h"

#include "util/PresetSerializationScope.h"
#include "util/Serialization.h"

#ifdef _DEBUG
    //#define DATASET_PICKER_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

DatasetPickerAction::DatasetPickerAction(QObject* parent, const QString& title) :
    OptionAction(parent, title),
    _populationMode(AbstractDatasetsModel::PopulationMode::Automatic),
    _datasetsListModel(AbstractDatasetsModel::PopulationMode::Manual),
    _blockDatasetsChangedSignal(false)
{
    setText(title);
    setIconByName("database");
    setToolTip("Pick a dataset");
    setPlaceHolderString("--choose dataset--");
    setCustomModel(&_datasetsFilterModel);

    connect(this, &OptionAction::currentIndexChanged, this, [this](std::int32_t currentIndex) {
        const auto previousDataset = getDataset(_currentDatasetId);

        emit datasetAboutToBePicked(previousDataset);

        if (currentIndex < 0) {
            _currentDatasetId.clear();
            emit datasetPicked({});
            return;
        }

        const auto proxyIndex   = _datasetsFilterModel.index(currentIndex, 0);
        const auto sourceIndex  = _datasetsFilterModel.mapToSource(proxyIndex);

        if (!sourceIndex.isValid()) {
            _currentDatasetId.clear();
            emit datasetPicked({});
            return;
        }

        Dataset<DatasetImpl> dataset;

        switch (_populationMode) {
	        case AbstractDatasetsModel::PopulationMode::Manual:
	            dataset = _datasetsListModel.getDataset(sourceIndex.row());
	            break;

	        case AbstractDatasetsModel::PopulationMode::Automatic:
	            dataset = mv::data().getDatasetsListModel().getDataset(sourceIndex.row());
	            break;
        }

        _currentDatasetId = dataset.isValid() ? dataset->getId() : QString{};

        emit datasetPicked(dataset);
    });

    const auto filterModelChanged = [this]() -> void {
        if (isDatasetsChangedSignalBlocked())
            return;

        const auto datasets     = getDatasets();
        const auto datasetIds   = getCurrentDatasetIds();

        if (datasetIds == _currentDatasetsIds)
            return;

        _currentDatasetsIds = datasetIds;

        if (!_currentDatasetId.isEmpty()) {
            setCurrentDataset(datasetIds.contains(_currentDatasetId) ? _currentDatasetId : QString{});
        }

        emit datasetsChanged(datasets);
    };

    connect(&_datasetsFilterModel, &QSortFilterProxyModel::rowsInserted, this, filterModelChanged);
    connect(&_datasetsFilterModel, &QSortFilterProxyModel::rowsRemoved, this, filterModelChanged);
    connect(&_datasetsFilterModel, &QSortFilterProxyModel::dataChanged, this, filterModelChanged);
    connect(&_datasetsFilterModel, &QSortFilterProxyModel::layoutChanged, this, filterModelChanged);

    populationModeChanged();
}

Datasets DatasetPickerAction::getDatasets() const
{
    Datasets datasets;

    for (std::int32_t filterModelRowIndex = 0; filterModelRowIndex < _datasetsFilterModel.rowCount(); ++filterModelRowIndex) {
        const auto sourceModelIndex = _datasetsFilterModel.mapToSource(_datasetsFilterModel.index(filterModelRowIndex, 0));

        if (!sourceModelIndex.isValid())
            continue;

        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                datasets << _datasetsListModel.getDataset(sourceModelIndex.row());
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                datasets << mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());
                break;
        }
    }

    return datasets;
}

void DatasetPickerAction::setDatasets(Datasets datasets, bool silent /*= false*/)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setPopulationMode(AbstractDatasetsModel::PopulationMode::Manual);

    if (silent)
        blockDatasetsChangedSignal();

    _datasetsListModel.setDatasets(std::move(datasets));

    if (silent) {
        unblockDatasetsChangedSignal();

        _currentDatasetsIds = getCurrentDatasetIds();

        if (!_currentDatasetId.isEmpty() &&
            !_currentDatasetsIds.contains(_currentDatasetId)) {
            setCurrentDataset(QString{});
        }
    }

    if (auto* publicAction =
        dynamic_cast<DatasetPickerAction*>(getPublicAction())) {
        setCurrentDataset(publicAction->getCurrentDataset());
    }
}

void DatasetPickerAction::setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _datasetsFilterModel.setFilterFunction(filterFunction);
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset() const
{
    return getDataset(_currentDatasetId);
}

void DatasetPickerAction::setCurrentDataset(Dataset<DatasetImpl> currentDataset)
{
    setCurrentDataset(currentDataset.isValid() ? currentDataset->getId() : QString{});
}

void DatasetPickerAction::setCurrentDataset(const QString& datasetId)
{
    if (datasetId.isEmpty()) {
        _currentDatasetId.clear();
        setCurrentIndex(-1);
        return;
    }

    QModelIndex datasetIndex;

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            datasetIndex = _datasetsListModel.getIndexFromDataset(datasetId);
            break;

        case AbstractDatasetsModel::PopulationMode::Automatic:
            datasetIndex = mv::data().getDatasetsListModel().getIndexFromDataset(datasetId);
            break;
    }

    if (!datasetIndex.isValid())
        return;

    const auto proxyIndex = _datasetsFilterModel.mapFromSource(datasetIndex);

    if (!proxyIndex.isValid()) {
        setCurrentDataset(QString{});
        return;
    }

    setCurrentIndex(proxyIndex.row());
}

QString DatasetPickerAction::getCurrentDatasetId() const
{
    return _currentDatasetId;
}

QStringList DatasetPickerAction::getCurrentDatasetIds() const
{
	QStringList datasetIds;

	for (const auto& dataset : getDatasets()) {
		if (dataset.isValid())
			datasetIds << dataset->getId();
	}

	return datasetIds;
}

Dataset<DatasetImpl> DatasetPickerAction::getDataset(const QString& datasetId) const
{
	if (datasetId.isEmpty())
		return {};

	QModelIndex datasetIndex;

	switch (_populationMode) {
		case AbstractDatasetsModel::PopulationMode::Manual: {
			datasetIndex = _datasetsListModel.getIndexFromDataset(datasetId);

			return datasetIndex.isValid() ? _datasetsListModel.getDataset(datasetIndex.row()) : Dataset<DatasetImpl>{};
        }

		case AbstractDatasetsModel::PopulationMode::Automatic: {
			const auto& model = mv::data().getDatasetsListModel();

			datasetIndex = model.getIndexFromDataset(datasetId);

			return datasetIndex.isValid() ? model.getDataset(datasetIndex.row()) : Dataset<DatasetImpl>{};
		}
	}

	return {};
}

void DatasetPickerAction::invalidateFilter()
{
    _datasetsFilterModel.invalidate();
}

AbstractDatasetsModel::PopulationMode DatasetPickerAction::getPopulationMode() const
{
    return _populationMode;
}

void DatasetPickerAction::setPopulationMode(AbstractDatasetsModel::PopulationMode populationMode)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto previousPopulationMode = _populationMode;

    if (populationMode == _populationMode)
        return;

    _populationMode = populationMode;

    populationModeChanged();

    emit populationModeChanged(previousPopulationMode, _populationMode);
}

void DatasetPickerAction::populationModeChanged()
{
    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual: {
            _datasetsFilterModel.getUseFilterFunctionAction().setChecked(false);
            _datasetsFilterModel.setSourceModel(&_datasetsListModel);
            break;
        }

        case AbstractDatasetsModel::PopulationMode::Automatic:
        {
            _datasetsFilterModel.getUseFilterFunctionAction().setChecked(true);
            _datasetsFilterModel.setSourceModel(&const_cast<DatasetsListModel&>(mv::data().getDatasetsListModel()));
            break;
        }
    }
}

void DatasetPickerAction::blockDatasetsChangedSignal()
{
    _blockDatasetsChangedSignal = true;
}

void DatasetPickerAction::unblockDatasetsChangedSignal()
{
    _blockDatasetsChangedSignal = false;
}

bool DatasetPickerAction::isDatasetsChangedSignalBlocked() const
{
    return _blockDatasetsChangedSignal;
}

void DatasetPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(publicAction);

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    if (publicDatasetPickerAction == nullptr)
        return;

    connect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    connect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());

    OptionAction::connectToPublicAction(publicAction, recursive);
}

void DatasetPickerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    Q_ASSERT(publicDatasetPickerAction != nullptr);

    if (publicDatasetPickerAction == nullptr)
        return;

    disconnect(this, &DatasetPickerAction::datasetPicked, publicDatasetPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));
    disconnect(publicDatasetPickerAction, &DatasetPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&DatasetPickerAction::setCurrentDataset));

    OptionAction::disconnectFromPublicAction(recursive);
}

void DatasetPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    OptionAction::fromVariantMap(variantMap);

    if (PresetSerializationScope::isActive())
        return;

    variantMapMustContain(variantMap, "Value");

    setCurrentDataset(variantMap["Value"].toString());
}

QVariantMap DatasetPickerAction::toVariantMap() const
{
    auto variantMap = OptionAction::toVariantMap();

	if (!PresetSerializationScope::isActive()) {
        variantMap.insert({
            { "Value", getCurrentDatasetId() }
        });
    }

    return variantMap;
}

}
