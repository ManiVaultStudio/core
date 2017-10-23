#ifndef HDPS_PLUGIN_PLUGIN_MANAGER_H
#define HDPS_PLUGIN_PLUGIN_MANAGER_H

#include "Core.h"

#include "PluginFactory.h"

#include <QObject>
#include <QString>
#include <QHash>
#include <QDir>

namespace Ui
{
    class MainWindow;
}

namespace hdps
{
namespace plugin
{

class PluginManager : public QObject {
public:
    PluginManager(Core& core);
    ~PluginManager(void);
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void loadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core.
    */
    QString createPlugin(const QString kind);
    
private:
    /**
    * Resolves plugin dependencies, returns list of resolved plugin names.
    */
    QVector<QString> resolveDependencies(QDir pluginDir) const;

    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;
private slots:
    QString pluginTriggered(const QString& kind);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_MANAGER_H
