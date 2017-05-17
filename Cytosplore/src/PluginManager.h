#ifndef HDPS_PLUGIN_PLUGIN_MANAGER_H
#define HDPS_PLUGIN_PLUGIN_MANAGER_H

#include "Core.h"

#include "PluginFactory.h"

#include <QObject>
#include <QString>
#include <QHash>

namespace Ui
{
    class MainWindow;
}

namespace hdps {

namespace plugin {

class PluginManager : public QObject {
    
public:
    PluginManager(Core& core);
    ~PluginManager(void);
    
    void LoadPlugins();
    Plugin* AddPlugin(const QString kind);
    
private:
    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;
private slots:
    Plugin* pluginTriggered(const QString& kind);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_MANAGER_H
