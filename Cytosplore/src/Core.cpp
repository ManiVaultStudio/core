#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "DataTypePlugin.h"
#include "DataConsumer.h"

namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {
    _pluginManager.reset();

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
        _mainWindow.addView(plugin);
    }

    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));

    // Initialize the plugin after it has been added to the core
    plugin->init();

    // Notify data consumers about the new data set
    if (plugin->getType() == plugin::Type::DATA_TYPE) {
        notifyDataAdded(*dynamic_cast<plugin::DataTypePlugin*>(plugin));
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

plugin::DataTypePlugin* Core::addData(const QString kind) {
    return (plugin::DataTypePlugin*) _pluginManager->AddPlugin(kind);
}

void Core::notifyDataAdded(const plugin::DataTypePlugin& data) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataAdded(data);
            }
        }
    }
}

void Core::notifyDataChanged(const plugin::DataTypePlugin& data) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataChanged(data);
            }
        }
    }
}

void Core::notifyDataRemoved() {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataRemoved();
            }
        }
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
