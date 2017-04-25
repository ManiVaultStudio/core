#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {

}

void Core::init() {
    _pluginManager = std::unique_ptr<plugin::PluginManager>(new plugin::PluginManager(*this));

    _pluginManager->LoadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin) {
    // If is it a view plugin then it should be added to the main window
    if (plugin->getType() == plugin::Type::VIEW) {
        _mainWindow.addView(plugin);
    }

    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
