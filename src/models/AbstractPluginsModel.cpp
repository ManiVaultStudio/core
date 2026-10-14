// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractPluginsModel.h"

#include "CoreInterface.h"
#include "Plugin.h"

using namespace mv::gui;

#ifdef _DEBUG
    //#define ABSTRACT_PLUGINS_MODEL_VERBOSE
#endif

Q_DECLARE_METATYPE(mv::plugin::Plugin*);

namespace mv {

AbstractPluginsModel::Item::Item(plugin::Plugin* plugin, const QString& title) :
    QStandardItem(title),
    _plugin(plugin)
{
}

QVariant AbstractPluginsModel::Item::data(int role) const
{
    switch (role)
    {
        case Qt::UserRole + 1:
            return QVariant::fromValue(getPlugin());

        default:
            break;
    }

    return QStandardItem::data(role);
}

plugin::Plugin* AbstractPluginsModel::Item::getPlugin() const
{
    return _plugin;
}

QVariant AbstractPluginsModel::IdItem::data(int role) const
{
    switch (role)
    {
        case Qt::DisplayRole:
        case Qt::EditRole:
        case Qt::ToolTipRole:
            return getPlugin() ? getPlugin()->getId() : "";

        default:
            break;
    }

    return Item::data(role);
}

AbstractPluginsModel::AbstractPluginsModel(PopulationMode populationMode /*= PopulationMode::Automatic*/, QObject* parent /*= nullptr*/) :

    StandardItemModel(parent),

    _populationMode()

{
    setPopulationMode(populationMode);
}

AbstractPluginsModel::PopulationMode AbstractPluginsModel::getPopulationMode() const
{
    return _populationMode;
}

void AbstractPluginsModel::setPopulationMode(PopulationMode populationMode)
{
#ifdef ABSTRACT_PLUGINS_MODEL_VERBOSE
        qDebug() << __FUNCTION__;
#endif

    _populationMode = populationMode;

    switch (_populationMode)
    {
        case PopulationMode::Manual: {
            disconnect(&mv::plugins(), &AbstractPluginManager::pluginAdded, this, nullptr);
            disconnect(&mv::plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, nullptr);

            break;
        }

        case PopulationMode::Automatic: {
            connect(&mv::plugins(), &AbstractPluginManager::pluginAdded, this, &AbstractPluginsModel::addPlugin);
            connect(&mv::plugins(), &AbstractPluginManager::pluginAboutToBeDestroyed, this, &AbstractPluginsModel::removePlugin);

            break;
        }
    }
}

QVariant AbstractPluginsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::Category:
            return CategoryItem::headerData(orientation, role);

        case Column::Id:
            return IdItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}