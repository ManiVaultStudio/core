// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetPickerAction.h"

#include "event/Event.h"

#ifdef _DEBUG
    //#define DATASET_PICKER_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

bool DatasetPickerAction::noValueSerialization = false;

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

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        const auto sourceModelRow = _datasetsFilterModel.mapToSource(_datasetsFilterModel.index(currentIndex, 0)).row();

        emit datasetAboutToBePicked(getCurrentDataset());

        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                emit datasetPicked(_datasetsListModel.getDataset(sourceModelRow));
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                emit datasetPicked(mv::data().getDatasetsListModel().getDataset(sourceModelRow));
                break;
        }
    });

    const auto filterModelChanged = [this]() -> void {
        if (isDatasetsChangedSignalBlocked())
            return;

        const auto datasets = getDatasets();

        QStringList datasetsIds;

        for (const auto& dataset : datasets)
            datasetsIds << dataset->getId();

        if (datasetsIds == _currentDatasetsIds)
            return;

        _currentDatasetsIds = datasetsIds;

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

    if (silent) {
        blockDatasetsChangedSignal();
        {
            _datasetsListModel.setDatasets(datasets);
        }
        unblockDatasetsChangedSignal();
    }
    else {
        _datasetsListModel.setDatasets(datasets);

        emit datasetsChanged(_datasetsListModel.getDatasets());
    }

    if (auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction()))
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
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
    if (getCurrentIndex() < 0)
        return {};

    const auto filterModelIndex = _datasetsFilterModel.index(getCurrentIndex(), 0);

    if (!filterModelIndex.isValid())
        return {};

    const auto sourceModelIndex = _datasetsFilterModel.mapToSource(filterModelIndex);

    if (!sourceModelIndex.isValid())
        return {};

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            return _datasetsListModel.getDataset(sourceModelIndex.row());

        case AbstractDatasetsModel::PopulationMode::Automatic:
            return mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());
    }

    return {};
}

void DatasetPickerAction::setCurrentDataset(Dataset<DatasetImpl> currentDataset)
{
    QModelIndex datasetIndex;

    if (currentDataset.isValid()) {
        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                datasetIndex = _datasetsListModel.getIndexFromDataset(currentDataset);
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                datasetIndex = mv::data().getDatasetsListModel().getIndexFromDataset(currentDataset);
                break;
        }
    }

    if (!datasetIndex.isValid())
        return;

    setCurrentIndex(_datasetsFilterModel.mapFromSource(datasetIndex).row());
}

void DatasetPickerAction::setCurrentDataset(const QString& datasetId)
{
    if (datasetId.isEmpty())
        return;

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

    setCurrentIndex(_datasetsFilterModel.mapFromSource(datasetIndex).row());
}

QString DatasetPickerAction::getCurrentDatasetId() const
{
    return getCurrentDataset().getDatasetId();
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

    WidgetAction::connectToPublicAction(publicAction, recursive);
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

    WidgetAction::disconnectFromPublicAction(recursive);
}

void DatasetPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    if (isValueSerializationDisabled())
        return;

    variantMapMustContain(variantMap, "Value");

    setCurrentDataset(variantMap["Value"].toString());
}

QVariantMap DatasetPickerAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    if (!isValueSerializationDisabled()) {
        variantMap.insert({
            { "Value", getCurrentDatasetId() }
        });
    }

    return variantMap;
}

bool DatasetPickerAction::isValueSerializationDisabled()
{
    return noValueSerialization;
}

void DatasetPickerAction::setValueSerializationDisabled(bool valueSerializationDisabled)
{
    noValueSerialization = valueSerializationDisabled;
}

void DatasetPickerAction::disableValueSerialization()
{
    setValueSerializationDisabled(true);
}

void DatasetPickerAction::enableValueSerialization()
{
    setValueSerializationDisabled(false);
}

}
