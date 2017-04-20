#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include <memory>

namespace hdps {

namespace plugin {
    class PluginManager;
}

namespace gui {
    class MainWindow;
}

class Core
{
public:
    Core(gui::MainWindow& mainWindow);
    ~Core();

    void init();

    gui::MainWindow& gui() const;

private:
    gui::MainWindow& _mainWindow;

    std::unique_ptr<plugin::PluginManager> _pluginManager;
};

} // namespace hdps

#endif // HDPS_CORE_H
