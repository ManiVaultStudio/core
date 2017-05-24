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
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void LoadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core.
    */
    QString AddPlugin(const QString kind);
    
private:
    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;
private slots:
    QString pluginTriggered(const QString& kind);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_MANAGER_H
