// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginManagerModel.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractPluginManager.h>

using namespace hdps;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_MODEL_VERBOSE
#endif

Q_DECLARE_METATYPE(hdps::plugin::Plugin*);

QMap<PluginManagerModel::Column, QPair<QString, QString>> PluginManagerModel::columns = QMap<PluginManagerModel::Column, QPair<QString, QString>>({
    { PluginManagerModel::Column::Name, { "Name", "Item name (plugin type, factory name or plugin name)" }},
    { PluginManagerModel::Column::Category, { "Category", "Item category (type, factory or instance)" }},
    { PluginManagerModel::Column::ID, { "ID", "Globally unique plugin instance identifier" }}
});

PluginManagerModel::PluginManagerModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    synchronizeWithPluginManager();

    for (const auto& column : columns.keys()) {
        setHeaderData(static_cast<int>(column), Qt::Horizontal, columns[column].first);
        setHeaderData(static_cast<int>(column), Qt::Horizontal, columns[column].second, Qt::ToolTipRole);
    }
}

void PluginManagerModel::synchronizeWithPluginManager()
{
#ifdef PLUGIN_MANAGER_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto pluginTypes = plugin::Types {
        plugin::Type::ANALYSIS,
        plugin::Type::DATA,
        plugin::Type::LOADER,
        plugin::Type::WRITER,
        plugin::Type::TRANSFORMATION,
        plugin::Type::VIEW
    };

    for (auto pluginType : pluginTypes) {
        auto pluginTypeRow = new QStandardItem(getPluginTypeIcon(pluginType), QString("%1 plugins").arg(getPluginTypeName(pluginType)));
        
        pluginTypeRow->setEnabled(false);
        pluginTypeRow->setEditable(false);

        appendRow({ pluginTypeRow, new QStandardItem("Type"), new QStandardItem("") });

        for (auto pluginFactory : plugins().getPluginFactoriesByType(pluginType)) {
            auto pluginFactoryRow = new QStandardItem(pluginFactory->getIcon(), pluginFactory->getKind());

            pluginFactoryRow->setEnabled(false);
            pluginFactoryRow->setEditable(false);

            pluginTypeRow->appendRow({ pluginFactoryRow, new QStandardItem("Factory"), new QStandardItem("") });

            for (auto plugin : plugins().getPluginsByFactory(pluginFactory)) {
                auto pluginRow  = new QStandardItem(plugin->getGuiName());
                auto pluginId   = new QStandardItem(plugin->getId());
                
                pluginId->setEditable(false);

                pluginRow->setData(QVariant::fromValue(plugin));
                pluginRow->setEditable(false);

                pluginFactoryRow->appendRow({ pluginRow, new QStandardItem("Instance"), pluginId });

                pluginTypeRow->setEnabled(true);
                pluginFactoryRow->setEnabled(true);
            }
        }
    }
}
