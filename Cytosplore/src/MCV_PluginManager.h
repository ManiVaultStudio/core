#ifndef MCV_PLUGIN_MANAGER
#define MCV_PLUGIN_MANAGER

#include <QObject>

#include <QVector>

namespace Ui
{
    class MainWindow;
}

class MCV_PluginManager : public QObject {
    
public:
    MCV_PluginManager(void);
    ~MCV_PluginManager(void);
    
    void LoadPlugins(QSharedPointer<Ui::MainWindow> ui);
    
private:
    
    QVector< QObject* > _plugins;
    
private slots:
    void pluginTriggered(int idx);
};

#endif // MCV_PLUGIN_MANAGER
