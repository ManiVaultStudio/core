#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "DataTypePlugin.h"
#include "DataConsumer.h"

namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {
    // Delete the plugin manager
    _pluginManager.reset();

    // Delete all plugins
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            kv.second[i].reset();
        }
    }
}

void Core::init() {
    _pluginManager = std::unique_ptr<plugin::PluginManager>(new plugin::PluginManager(*this));

    _pluginManager->LoadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin) {
    plugin->setCore(this);

    // If it is a view plugin then it should be added to the main window
    if (plugin->getType() == plugin::Type::VIEW) {
        _mainWindow.addView(dynamic_cast<plugin::ViewPlugin*>(plugin));
    }

    // Add the plugin to a list of plugins of the same type
    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));

    // Initialize the plugin after it has been added to the core
    plugin->init();

    // Notify data consumers about the new data set
    if (plugin->getType() == plugin::Type::DATA_TYPE) {
        notifyDataAdded(plugin->getName());
    }

    // If it is a loader plugin it should call loadData
    if (plugin->getType() == plugin::Type::LOADER) {
        dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
    }
    // If it is a writer plugin it should call writeData
    if (plugin->getType() == plugin::Type::WRITER) {
        dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();
    }
}

/**
 * Requests the plugin manager to create a new plugin of the given kind.
 * The manager will add the plugin instance to the core and return the
 * unique name of the plugin.
 */
const QString Core::addData(const QString kind) {
    return _pluginManager->AddPlugin(kind);
}

/**
 * Requests an instance of a data type plugin from the core which has the same
 * unique name as the given parameter. If no such instance can be found a fatal
 * error is thrown.
 */
plugin::DataTypePlugin* Core::requestData(const QString name)
{
    for (std::unique_ptr<plugin::Plugin>& plugin : _plugins[plugin::Type::DATA_TYPE]) {
        if (plugin->getName() == name) {
            return dynamic_cast<plugin::DataTypePlugin*>(plugin.get());
        }
    }
    qFatal((QString("Failed to find plugin with name: ") + name).toStdString().c_str());
}

/**
 * Goes through all plugins stored in the core and calls the 'dataAdded' function
 * on all plugins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString name) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataAdded(name);
            }
        }
    }
}

/**
* Goes through all plugins stored in the core and calls the 'dataChanged' function
* on all plugins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString name) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataChanged(name);
            }
        }
    }
}

/**
* Goes through all plugins stored in the core and calls the 'dataRemoved' function
* on all plugins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString name) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataRemoved(name);
            }
        }
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
