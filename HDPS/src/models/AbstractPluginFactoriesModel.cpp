// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractPluginsModel.h"
#include "PluginFactory.h"

using namespace mv;
using namespace mv::plugin;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_MODEL_VERBOSE
#endif

Q_DECLARE_METATYPE(mv::plugin::Plugin*);

/*
QMap<PluginManagerModel::Column, QPair<QString, QString>> PluginManagerModel::columns = QMap<PluginManagerModel::Column, QPair<QString, QString>>({
    { PluginManagerModel::Column::Name, { "Name", "Item name (plugin type, factory name or plugin name)" }},
    { PluginManagerModel::Column::Category, { "Category", "Item category (type, factory or instance)" }},
    { PluginManagerModel::Column::ID, { "ID", "Globally unique plugin instance identifier" }}
});
*/

namespace mv {

//AbstractPluginsModel::AbstractPluginsModel(QObject* parent /*= nullptr*/) :
//    QStandardItemModel(parent)
//{
//}
//
//
//AbstractPluginsModel::Item::Item(plugin::PluginFactory* pluginFactory) :
//    QStandardItem(),
//    _pluginFactory(pluginFactory)
//{
//}
//
//PluginFactory* AbstractPluginsModel::Item::getPluginFactory() const
//{
//    return _pluginFactory;
//}
//
//QVariant AbstractPluginsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
//{
//    switch (role) {
//        case Qt::EditRole:
//        case Qt::DisplayRole:
//            return getPluginFactory()->getKind();
//
//        case Qt::ToolTipRole:
//            return QString("%1").arg(data(Qt::DisplayRole).toString());
//
//        //case Qt::DecorationRole:
//        //{
//        //    if (!getDataset().isValid())
//        //        break;
//
//        //    return getDataset()->getIcon();
//        //}
//
//        default:
//            break;
//    }
//
//    return Item::data(role);
//}
//
//QVariant AbstractPluginsModel::CategoryItem::data(int role /*= Qt::UserRole + 1*/) const
//{
//    switch (role) {
//        case Qt::EditRole:
//        case Qt::DisplayRole:
//            return getPluginFactory()->getKind();
//
//        default:
//            break;
//    }
//
//    return Item::data(role);
//}
//
//QVariant AbstractPluginsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
//{
//
//}
//
//AbstractPluginsModel::Row::Row(plugin::PluginFactory* pluginFactory)
//{
//
//}

}