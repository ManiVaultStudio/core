// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginPickerAction.h"
#include "Application.h"
#include "DataHierarchyItem.h"

#include "event/Event.h"

#include <QHBoxLayout>

#ifdef _DEBUG
    //#define PLUGIN_PICKER_ACTION_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui {

PluginPickerAction::PluginPickerAction(QObject* parent, const QString& title) :
    OptionAction(parent, title),
    _populationMode(AbstractPluginsModel::PopulationMode::Automatic),
    _pluginsListModel(AbstractPluginsModel::PopulationMode::Manual),
    _blockDatasetsChangedSignal(false)
{
    setText(title);
    setIconByName("plug");
    setToolTip("Pick a plugin");
    setPlaceHolderString("--choose plugin--");
    setCustomModel(&_pluginsFilterModel);

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        const auto sourceModelRow = _pluginsFilterModel.mapToSource(_pluginsFilterModel.index(currentIndex, 0)).row();

        /*
        switch (_populationMode)
        {
            case AbstractPluginsModel::PopulationMode::Manual:
                emit pluginPicked(_pluginsListModel.getPlugin(sourceModelRow));
                break;

            case AbstractPluginsModel::PopulationMode::Automatic:
                emit pluginPicked(_pluginsListModel.getPlugin(sourceModelRow));
                break;

            default:
                break;
        }
        */
    });

    const auto filterModelChanged = [this]() -> void {
        /*
        if (isDatasetsChangedSignalBlocked())
            return;

        const auto datasets = getDatasets();

        QStringList datasetsIds;

        for (const auto& dataset : datasets)
            datasetsIds << dataset->getId();

        if (datasetsIds == _currentDatasetsIds)
            return;

        _currentDatasetsIds = datasetsIds;

        emit pluginsChanged(datasets);
        */
    };

    connect(&_pluginsFilterModel, &QSortFilterProxyModel::rowsInserted, this, filterModelChanged);
    connect(&_pluginsFilterModel, &QSortFilterProxyModel::rowsRemoved, this, filterModelChanged);
    connect(&_pluginsFilterModel, &QSortFilterProxyModel::dataChanged, this, filterModelChanged);
    connect(&_pluginsFilterModel, &QSortFilterProxyModel::layoutChanged, this, filterModelChanged);

    populationModeChanged();
}

Datasets PluginPickerAction::getDatasets() const
{
    
    Datasets datasets;

    /*
    for (std::int32_t filterModelRowIndex = 0; filterModelRowIndex < _pluginsFilterModel.rowCount(); ++filterModelRowIndex) {
        const auto sourceModelIndex = _pluginsFilterModel.mapToSource(_pluginsFilterModel.index(filterModelRowIndex, 0));

        if (!sourceModelIndex.isValid())
            continue;

        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                datasets << _pluginsListModel.getDataset(sourceModelIndex.row());
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                datasets << mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());
                break;

            default:
                break;
        }
    }
    */
    return datasets;
}

void PluginPickerAction::setDatasets(Datasets datasets, bool silent /*= false*/)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    /*
    setPopulationMode(AbstractDatasetsModel::PopulationMode::Manual);

    if (silent) {
        blockPluginsChangedSignal();
        {
            _pluginsListModel.setDatasets(datasets);
        }
        unblockPluginsChangedSignal();
    }
    else {
        _pluginsListModel.setDatasets(datasets);

        emit datasetsChanged(_pluginsListModel.getDatasets());
    }

    auto publicPluginPickerAction = dynamic_cast<PluginPickerAction*>(getPublicAction());

    if (publicPluginPickerAction)
        setCurrentDataset(publicPluginPickerAction->getCurrentDataset());
    */
}

void PluginPickerAction::setFilterFunction(const DatasetsFilterModel::FilterFunction& filterFunction)
{
#ifdef DATASET_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    //_pluginsFilterModel.setFilterFunction(filterFunction);
}

Dataset<DatasetImpl> PluginPickerAction::getCurrentDataset() const
{
    /*
    if (getCurrentIndex() < 0)
        return {};

    const auto filterModelIndex = _pluginsFilterModel.index(getCurrentIndex(), 0);

    if (!filterModelIndex.isValid())
        return {};

    const auto sourceModelIndex = _pluginsFilterModel.mapToSource(filterModelIndex);

    if (!sourceModelIndex.isValid())
        return {};

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            return _pluginsListModel.getDataset(sourceModelIndex.row());

        case AbstractDatasetsModel::PopulationMode::Automatic:
            return mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());

        default:
            break;
    }
    */

    return {};
}

void PluginPickerAction::setCurrentDataset(Dataset<DatasetImpl> currentDataset)
{
    /*
    QModelIndex datasetIndex;

    if (currentDataset.isValid()) {
        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                datasetIndex = _pluginsListModel.getIndexFromDataset(currentDataset);
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

    setCurrentIndex(_pluginsFilterModel.mapFromSource(datasetIndex).row());
    */
}

void PluginPickerAction::setCurrentDataset(const QString& datasetId)
{
    /*
    if (datasetId.isEmpty())
        return;

    QModelIndex datasetIndex;

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            datasetIndex = _pluginsListModel.getIndexFromDataset(datasetId);
            break;

        case AbstractDatasetsModel::PopulationMode::Automatic:
            datasetIndex = mv::data().getDatasetsListModel().getIndexFromDataset(datasetId);
            break;

        default:
            break;
    }

    if (!datasetIndex.isValid())
        return;

    setCurrentIndex(_pluginsFilterModel.mapFromSource(datasetIndex).row());
    */
}

QString PluginPickerAction::getCurrentDatasetId() const
{
    return getCurrentDataset().getDatasetId();
}

AbstractPluginsModel::PopulationMode PluginPickerAction::getPopulationMode() const
{
    return _populationMode;
}

void PluginPickerAction::setPopulationMode(AbstractPluginsModel::PopulationMode populationMode)
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

void PluginPickerAction::populationModeChanged()
{
    /*
    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual: {
            _pluginsFilterModel.getUseFilterFunctionAction().setChecked(false);
            _pluginsFilterModel.setSourceModel(&_pluginsListModel);
            break;
        }

        case AbstractDatasetsModel::PopulationMode::Automatic:
        {
            _pluginsFilterModel.getUseFilterFunctionAction().setChecked(true);
            _pluginsFilterModel.setSourceModel(&const_cast<DatasetsListModel&>(mv::data().getDatasetsListModel()));
            break;
        }

        default:
            break;
    }
    */
}

void PluginPickerAction::blockPluginsChangedSignal()
{
    _blockDatasetsChangedSignal = true;
}

void PluginPickerAction::unblockPluginsChangedSignal()
{
    _blockDatasetsChangedSignal = false;
}

bool PluginPickerAction::isDatasetsChangedSignalBlocked() const
{
    return _blockDatasetsChangedSignal;
}

void PluginPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    /*
    auto publicPluginPickerAction = dynamic_cast<PluginPickerAction*>(publicAction);

    Q_ASSERT(publicPluginPickerAction != nullptr);

    if (publicPluginPickerAction == nullptr)
        return;

    connect(this, &PluginPickerAction::datasetPicked, publicPluginPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentDataset));
    connect(publicPluginPickerAction, &PluginPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentDataset));

    setCurrentDataset(publicPluginPickerAction->getCurrentDataset());

    WidgetAction::connectToPublicAction(publicAction, recursive);
    */
}

void PluginPickerAction::disconnectFromPublicAction(bool recursive)
{
    /*
    if (!isConnected())
        return;

    auto publicPluginPickerAction = dynamic_cast<PluginPickerAction*>(getPublicAction());

    Q_ASSERT(publicPluginPickerAction != nullptr);

    if (publicPluginPickerAction == nullptr)
        return;

    disconnect(this, &PluginPickerAction::datasetPicked, publicPluginPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentDataset));
    disconnect(publicPluginPickerAction, &PluginPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentDataset));

    WidgetAction::disconnectFromPublicAction(recursive);
    */
}

void PluginPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    OptionAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setCurrentDataset(variantMap["Value"].toString());
}

QVariantMap PluginPickerAction::toVariantMap() const
{
    QVariantMap variantMap = OptionAction::toVariantMap();

    variantMap.insert({
        { "Value", getCurrentDatasetId() }
    });

    return variantMap;
}

}
