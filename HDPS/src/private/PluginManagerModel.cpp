#include "PluginManagerModel.h"

#include <Application.h>
#include <AbstractPluginManager.h>

using namespace hdps;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_MODEL_VERBOSE
#endif

PluginManagerModel::PluginManagerModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    initializeFromPluginManager();

    setHeaderData(0, Qt::Horizontal, "Name");
}

void PluginManagerModel::initializeFromPluginManager()
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
        
        appendRow(pluginTypeRow);

        for (auto pluginFactory : Application::core()->getPluginManager().getPluginFactoriesByType(pluginType)) {
            auto pluginFactoryRow = new QStandardItem(pluginFactory->getIcon(), pluginFactory->getKind());

            pluginTypeRow->appendRow(pluginFactoryRow);

            for (auto plugin : Application::core()->getPluginManager().getPluginsByFactory(pluginFactory)) {
                auto pluginRow = new QStandardItem(plugin->getGuiName());

                pluginFactoryRow->appendRow(pluginRow);
            }
        }
    }
}
