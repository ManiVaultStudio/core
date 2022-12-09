#include "PluginManagerModel.h"

#include <Application.h>
#include <AbstractPluginManager.h>

using namespace hdps;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_MODEL_VERBOSE
#endif

Q_DECLARE_METATYPE(hdps::plugin::Plugin*);

PluginManagerModel::PluginManagerModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    synchronizeWithPluginManager();

    setHeaderData(0, Qt::Horizontal, "Name");
    setHeaderData(1, Qt::Horizontal, "Category");

    setColumnCount(2);
}

void PluginManagerModel::removeItem(const QModelIndex& index)
{
    beginRemoveRows(index.parent(), index.row(), index.row());
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

        appendRow({ pluginTypeRow, new QStandardItem("Type") });

        for (auto pluginFactory : Application::core()->getPluginManager().getPluginFactoriesByType(pluginType)) {
            auto pluginFactoryRow = new QStandardItem(pluginFactory->getIcon(), pluginFactory->getKind());

            pluginFactoryRow->setEnabled(false);

            pluginTypeRow->appendRow({ pluginFactoryRow, new QStandardItem("Factory") });

            for (auto plugin : Application::core()->getPluginManager().getPluginsByFactory(pluginFactory)) {
                auto pluginRow = new QStandardItem(plugin->getGuiName());
                
                pluginRow->setData(QVariant::fromValue(plugin));

                pluginFactoryRow->appendRow({ pluginRow, new QStandardItem("Instance") });

                pluginTypeRow->setEnabled(true);
                pluginFactoryRow->setEnabled(true);
            }
        }
    }
}
