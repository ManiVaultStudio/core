#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace hdps {

namespace plugin {
    class PluginManager;
    class Plugin;
    enum Type;
}

namespace gui {
    class MainWindow;
}

class Core : public CoreInterface
{
public:
    Core(gui::MainWindow& mainWindow);
    ~Core();

    void init();

    void addPlugin(plugin::Plugin* plugin);

    gui::MainWindow& gui() const;

private:
    gui::MainWindow& _mainWindow;

    std::unique_ptr<plugin::PluginManager> _pluginManager;

    std::unordered_map<plugin::Type, std::vector<std::unique_ptr<plugin::Plugin>>> _plugins;
};

} // namespace hdps

#endif // HDPS_CORE_H
