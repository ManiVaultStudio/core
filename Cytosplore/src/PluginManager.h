#ifndef HDPS_PLUGIN_PLUGIN_MANAGER_H
#define HDPS_PLUGIN_PLUGIN_MANAGER_H

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
    PluginManager(void);
    ~PluginManager(void);
    
    void LoadPlugins(QSharedPointer<Ui::MainWindow> ui);
    
private:
    
    QVector< QObject* > _plugins;
    
private slots:
    void pluginTriggered(int idx);
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_PLUGIN_MANAGER_H
