// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

#ifdef _DEBUG
    //#define DATASET_PICKER_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

DatasetPickerAction::DatasetPickerAction(QObject* parent, const QString& title) :
    OptionAction(parent, title),
    _populationMode(AbstractDatasetsModel::PopulationMode::Automatic),
    _datasetsListModel(AbstractDatasetsModel::PopulationMode::Manual),
    _datasetsFilterModel()
{
    setText(title);
    setIconByName("database");
    setToolTip("Pick a dataset");
    setPlaceHolderString("--choose dataset--");
    setCustomModel(&_datasetsFilterModel);

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        const auto sourceModelRow = _datasetsFilterModel.mapToSource(_datasetsFilterModel.index(currentIndex, 0)).row();

        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                emit datasetPicked(_datasetsListModel.getDataset(sourceModelRow));
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                emit datasetPicked(mv::data().getDatasetsListModel().getDataset(sourceModelRow));
                break;

            default:
                break;
        }
    });

    populationModeChanged();
}

Datasets DatasetPickerAction::getDatasets() const
{
    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            return _datasetsListModel.getDatasets();

        case AbstractDatasetsModel::PopulationMode::Automatic:
            return mv::data().getDatasetsListModel().getDatasets();

        default:
            break;
    }

    return {};
}

void DatasetPickerAction::setDatasets(Datasets datasets)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setPopulationMode(AbstractDatasetsModel::PopulationMode::Manual);

    _datasetsListModel.setDatasets(datasets);

    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    if (publicDatasetPickerAction)
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
}

void DatasetPickerAction::setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _datasetsFilterModel.setFilterFunction(filterFunction);

    setPopulationMode(AbstractDatasetsModel::PopulationMode::Automatic);
}

Dataset<DatasetImpl> DatasetPickerAction::getCurrentDataset() const
{
    if (getCurrentIndex() < 0)
        return {};

    const auto filterModelIndex = _datasetsFilterModel.index(getCurrentIndex(), 0);

    if (!filterModelIndex.isValid())
        return {};

    const auto sourceModelIndex = _datasetsFilterModel.mapToSource();

    if (!sourceModelIndex.isValid())
        return {};

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            return _datasetsListModel.getDataset(sourceModelIndex.row());

        case AbstractDatasetsModel::PopulationMode::Automatic:
            return mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());

        default:
            break;
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

            default:
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

        default:
            break;
    }

    if (!datasetIndex.isValid())
        return;

    setCurrentIndex(_datasetsFilterModel.mapFromSource(datasetIndex).row());
}

QString DatasetPickerAction::getCurrentDatasetGuid() const
{
    return getCurrentDataset().getDatasetId();
}


/*
    auto publicDatasetPickerAction = dynamic_cast<DatasetPickerAction*>(getPublicAction());

    if (publicDatasetPickerAction)
        setCurrentDataset(publicDatasetPickerAction->getCurrentDataset());
*/

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

        default:
            break;
    }
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

    variantMapMustContain(variantMap, "Value");

    setCurrentDataset(variantMap["Value"].toString());
}

QVariantMap DatasetPickerAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", getCurrentDatasetGuid() }
    });

    return variantMap;
}

}
