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

AbstractPluginFactoriesModel::Item::Item(const QString& type, plugin::PluginFactory* pluginFactory) :
    QStandardItem(),
    _type(type),
    _pluginFactory(pluginFactory)
{
    setEditable(false);
}

QString AbstractPluginFactoriesModel::Item::getType() const
{
    return _type;
}

PluginFactory* AbstractPluginFactoriesModel::Item::getPluginFactory() const
{
    return _pluginFactory;
}

QVariant AbstractPluginFactoriesModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getPluginFactory() ? getPluginFactory()->getKind() : getType();

        case Qt::ToolTipRole:
            return QString("%1").arg(data(Qt::DisplayRole).toString());

        case Qt::DecorationRole:
            return getPluginFactory() ? getPluginFactory()->getIcon() : QIcon();

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
            return getPluginFactory() ? QString("v%1").arg(getPluginFactory()->getVersion()) : "";

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractPluginFactoriesModel::NumberOfInstancesItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getPluginFactory() ? getPluginFactory()->getNumberOfInstances() : 0;

        case Qt::DisplayRole:
            return getPluginFactory() ? QString::number(data(Qt::EditRole).toInt()) : "";

        default:
            break;
    }

    return Item::data(role);
}

AbstractPluginFactoriesModel::Row::Row(const QString& type, plugin::PluginFactory* pluginFactory)
{
    append(new NameItem(type, pluginFactory));
    append(new VersionItem(type, pluginFactory));
    append(new NumberOfInstancesItem(type, pluginFactory));
}

AbstractPluginFactoriesModel::Row::Row(const QString& type) :
    Row(type, nullptr)
{
}

AbstractPluginFactoriesModel::Row::Row(plugin::PluginFactory* pluginFactory) :
    Row("", pluginFactory)
{
}

AbstractPluginFactoriesModel::AbstractPluginFactoriesModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant AbstractPluginFactoriesModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Name:
            return NameItem::headerData(orientation, role);

        case Column::Version:
            return VersionItem::headerData(orientation, role);

        case Column::NumberOfInstances:
            return NumberOfInstancesItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

}