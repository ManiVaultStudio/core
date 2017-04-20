#ifndef HDPS_PLUGIN_PLUGIN_MANAGER_H
#define HDPS_PLUGIN_PLUGIN_MANAGER_H

#include "Core.h"

#include "PluginFactory.h"

#include <QObject>
#include <QVector>

namespace Ui
{
    class MainWindow;
}

namespace hdps {

namespace plugin {

class PluginManager : public QObject {
    
public:
    PluginManager(const Core& core);
    ~PluginManager(void);
    
    void LoadPlugins();
    
private:
    const Core& _core;
    QVector< PluginFactory* > _plugins;
    
private slots:
    void pluginTriggered(int idx);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_MANAGER_H
