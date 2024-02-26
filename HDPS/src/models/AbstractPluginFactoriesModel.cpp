// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractPluginFactoriesModel.h"
#include "PluginFactory.h"

using namespace mv;
using namespace mv::plugin;

#ifdef _DEBUG
    #define PLUGIN_FACTORIES_MODEL_VERBOSE
#endif

namespace mv {

AbstractPluginFactoriesModel::Item::Item(plugin::PluginFactory* pluginFactory) :
    QStandardItem(),
    _pluginFactory(pluginFactory)
{
}

PluginFactory* AbstractPluginFactoriesModel::Item::getPluginFactory() const
{
    return _pluginFactory;
}

QVariant AbstractPluginFactoriesModel::KindItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getPluginFactory()->getKind();

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
            return getPluginFactory()->getIcon();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractPluginFactoriesModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getPluginFactory()->getKind();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractPluginFactoriesModel::VersionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getPluginFactory()->getKind();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractPluginFactoriesModel::NumberOfInstancesItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getPluginFactory()->getNumberOfInstances();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        default:
            break;
    }

    return Item::data(role);
}

AbstractPluginFactoriesModel::Row::Row(plugin::PluginFactory* pluginFactory)
{
    append(new KindItem(pluginFactory));
    append(new TypeItem(pluginFactory));
    append(new VersionItem(pluginFactory));
    append(new NumberOfInstancesItem(pluginFactory));
}

AbstractPluginFactoriesModel::AbstractPluginFactoriesModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

}