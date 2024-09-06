// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginPickerAction.h"

#include "event/Event.h"

#ifdef _DEBUG
    //#define PLUGIN_PICKER_ACTION_VERBOSE
#endif

using namespace mv::util;
using namespace mv::plugin;

namespace mv::gui {

PluginPickerAction::PluginPickerAction(QObject* parent, const QString& title) :
    OptionAction(parent, title),
    _populationMode(AbstractPluginsModel::PopulationMode::Automatic),
    _listModel(AbstractPluginsModel::PopulationMode::Manual),
    _blockPluginsChangedSignal(false)
{
    setText(title);
    setIconByName("plug");
    setToolTip("Pick a plugin");
    setPlaceHolderString("--choose plugin--");
    setCustomModel(&_filterModel);

    connect(this, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) {
        const auto sourceModelRow = _filterModel.mapToSource(_filterModel.index(currentIndex, 0)).row();

        switch (_populationMode)
        {
            case AbstractPluginsModel::PopulationMode::Manual:
                emit pluginPicked(_listModel.getPlugin(_listModel.index(sourceModelRow, 0)));
                break;

            case AbstractPluginsModel::PopulationMode::Automatic:
                emit pluginPicked(mv::plugins().getListModel().getPlugin(mv::plugins().getListModel().index(sourceModelRow, 0)));
                break;
        }
    });

    const auto filterModelChanged = [this]() -> void {
        if (isPluginsChangedSignalBlocked())
            return;

        const auto plugins = getPlugins();

        QStringList pluginIds;

        for (const auto& plugin : plugins)
            pluginIds << plugin->getId();

        if (pluginIds == _currentPluginsIds)
            return;

        _currentPluginsIds = pluginIds;

        emit pluginsChanged(plugins);
    };

    connect(&_filterModel, &QSortFilterProxyModel::rowsInserted, this, filterModelChanged);
    connect(&_filterModel, &QSortFilterProxyModel::rowsRemoved, this, filterModelChanged);
    connect(&_filterModel, &QSortFilterProxyModel::dataChanged, this, filterModelChanged);
    connect(&_filterModel, &QSortFilterProxyModel::layoutChanged, this, filterModelChanged);

    populationModeChanged();
}

Plugins PluginPickerAction::getPlugins() const
{
    Plugins plugins;

    /*
    for (std::int32_t filterModelRowIndex = 0; filterModelRowIndex < _filterModel.rowCount(); ++filterModelRowIndex) {
        const auto sourceModelIndex = _filterModel.mapToSource(_filterModel.index(filterModelRowIndex, 0));

        if (!sourceModelIndex.isValid())
            continue;

        switch (_populationMode)
        {
            case AbstractDatasetsModel::PopulationMode::Manual:
                datasets << _listModel.getDataset(sourceModelIndex.row());
                break;

            case AbstractDatasetsModel::PopulationMode::Automatic:
                datasets << mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());
                break;

            default:
                break;
        }
    }
    */

    return plugins;
}

void PluginPickerAction::setPlugins(const plugin::Plugins& plugins, bool silent /*= false*/)
{
#ifdef PLUGIN_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    setPopulationMode(AbstractPluginsModel::PopulationMode::Manual);

    if (silent) {
        blockPluginsChangedSignal();
        {
            _listModel.setPlugins(plugins);
        }
        unblockPluginsChangedSignal();
    }
    else {
        _listModel.setPlugins(plugins);

        emit pluginsChanged(_listModel.getPlugins());
    }

    if (auto publicPluginPickerAction = dynamic_cast<PluginPickerAction*>(getPublicAction()))
        setCurrentPlugin(publicPluginPickerAction->getCurrentPlugin());
}

void PluginPickerAction::setFilterFunction(const PluginsFilterModel::FilterFunction& filterFunction)
{
#ifdef PLUGIN_PICKER_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _filterModel.setFilterFunction(filterFunction);
}

plugin::Plugin* PluginPickerAction::getCurrentPlugin() const
{
    /*
    if (getCurrentIndex() < 0)
        return {};

    const auto filterModelIndex = _filterModel.index(getCurrentIndex(), 0);

    if (!filterModelIndex.isValid())
        return {};

    const auto sourceModelIndex = _filterModel.mapToSource(filterModelIndex);

    if (!sourceModelIndex.isValid())
        return {};

    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            return _listModel.getDataset(sourceModelIndex.row());

        case AbstractDatasetsModel::PopulationMode::Automatic:
            return mv::data().getDatasetsListModel().getDataset(sourceModelIndex.row());

        default:
            break;
    }
    */

    return {};
}

void PluginPickerAction::setCurrentPlugin(const plugin::Plugin* currentPlugin)
{
    Q_ASSERT(currentPlugin);

    if (!currentPlugin)
        return;

    QModelIndex pluginIndex;

    switch (_populationMode)
    {
        case AbstractPluginsModel::PopulationMode::Manual:
            pluginIndex = _listModel.getIndexFromPlugin(currentPlugin);
            break;

        case AbstractPluginsModel::PopulationMode::Automatic:
            //pluginIndex = mv::data().getDatasetsListModel().getIndexFromDataset(currentDataset);
            break;

        default:
            break;
    }

    if (!pluginIndex.isValid())
        return;

    setCurrentIndex(_filterModel.mapFromSource(pluginIndex).row());
}

void PluginPickerAction::setCurrentPlugin(const QString& pluginId)
{
    if (pluginId.isEmpty())
        return;

    QModelIndex datasetIndex;

    /*
    switch (_populationMode)
    {
        case AbstractDatasetsModel::PopulationMode::Manual:
            datasetIndex = _listModel.getIndexFromPlugin(datasetId);
            break;

        case AbstractDatasetsModel::PopulationMode::Automatic:
            datasetIndex = mv::data().getDatasetsListModel().getIndexFromPlugin(datasetId);
            break;

        default:
            break;
    }

    if (!datasetIndex.isValid())
        return;

    setCurrentIndex(_filterModel.mapFromSource(datasetIndex).row());
    */
}

QString PluginPickerAction::getCurrentPluginId() const
{
    if (auto currentPlugin = getCurrentPlugin())
        return currentPlugin->getId();

    return {};
}

AbstractPluginsModel::PopulationMode PluginPickerAction::getPopulationMode() const
{
    return _populationMode;
}

void PluginPickerAction::setPopulationMode(AbstractPluginsModel::PopulationMode populationMode)
{
#ifdef PLUGIN_PICKER_ACTION_VERBOSE
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
    switch (_populationMode)
    {
        case AbstractPluginsModel::PopulationMode::Manual:
        {
            _filterModel.getUseFilterFunctionAction().setChecked(false);
            _filterModel.setSourceModel(&_listModel);
            break;
        }

        case AbstractPluginsModel::PopulationMode::Automatic:
        {
            _filterModel.getUseFilterFunctionAction().setChecked(true);
            _filterModel.setSourceModel(&const_cast<DatasetsListModel&>(mv::data().getDatasetsListModel()));
            break;
        }

        default:
            break;
    }
}

void PluginPickerAction::blockPluginsChangedSignal()
{
    _blockPluginsChangedSignal = true;
}

void PluginPickerAction::unblockPluginsChangedSignal()
{
    _blockPluginsChangedSignal = false;
}

bool PluginPickerAction::isPluginsChangedSignalBlocked() const
{
    return _blockPluginsChangedSignal;
}

void PluginPickerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    /*
    auto publicPluginPickerAction = dynamic_cast<PluginPickerAction*>(publicAction);

    Q_ASSERT(publicPluginPickerAction != nullptr);

    if (publicPluginPickerAction == nullptr)
        return;

    connect(this, &PluginPickerAction::datasetPicked, publicPluginPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentPlugin));
    connect(publicPluginPickerAction, &PluginPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentPlugin));

    setCurrentPlugin(publicPluginPickerAction->getCurrentPlugin());

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

    disconnect(this, &PluginPickerAction::datasetPicked, publicPluginPickerAction, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentPlugin));
    disconnect(publicPluginPickerAction, &PluginPickerAction::datasetPicked, this, qOverload<mv::Dataset<mv::DatasetImpl>>(&PluginPickerAction::setCurrentPlugin));

    WidgetAction::disconnectFromPublicAction(recursive);
    */
}

void PluginPickerAction::fromVariantMap(const QVariantMap& variantMap)
{
    OptionAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setCurrentPlugin(variantMap["Value"].toString());
}

QVariantMap PluginPickerAction::toVariantMap() const
{
    QVariantMap variantMap = OptionAction::toVariantMap();

    variantMap.insert({
        { "Value", getCurrentPluginId() }
    });

    return variantMap;
}

}
