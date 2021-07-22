#ifndef HDPS_PLUGINMANAGER_H
#define HDPS_PLUGINMANAGER_H

#include "Core.h"

#include "PluginFactory.h"

#include <QObject>
#include <QString>
#include <QStringList>
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
    ~PluginManager(void) override;
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void loadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core.
    */
    QString createPlugin(const QString kind);
    
    /**
     * Create an analysis plugin
     * @param analysisKind Kind of analysis
     * @param inputDatasetName Name of the input dataset
     */
    void createAnalysisPlugin(const QString& kind, const QString& inputDatasetName);

private:
    /**
    * Resolves plugin dependencies, returns list of resolved plugin filenames.
    */
    QStringList resolveDependencies(QDir pluginDir) const;

    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;
private slots:
    QString pluginTriggered(const QString& kind);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGINMANAGER_H
