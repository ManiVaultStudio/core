#include "PluginManager.h"

#include <QApplication>
#include <QMenuBar>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QSignalMapper>

#include <assert.h>

#include "MainWindow.h"
#include "AnalysisPlugin.h"
#include "DataTypePlugin.h"
#include "LoaderPlugin.h"
#include "TransformationPlugin.h"
#include "ViewPlugin.h"
#include "PluginType.h"

namespace hdps {

namespace plugin {

PluginManager::PluginManager(const Core& core)
: _core(core)
{
    
}

PluginManager::~PluginManager(void)
{
    
}

void PluginManager::LoadPlugins()
{
	QDir pluginsDir(qApp->applicationDirPath());
    
#if defined(Q_OS_WIN)
    //if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
    //    pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("Plugins");
    
    _plugins.clear();
    
    QSignalMapper* signalMapper = new QSignalMapper(this);
    // for all items in the plugins directory
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        
        gui::MainWindow& gui = _core.gui();

        // create an instance of the plugin, i.e. the factory
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            _plugins.push_back(qobject_cast<PluginFactory*>(plugin));
            QAction* action = NULL;
            
            QString name = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();
            
            if ( qobject_cast<AnalysisPluginFactory*>(plugin) )
            {
                action = gui.addMenuAction(plugin::Type::ANALYSIS, name);
            }
            else if ( qobject_cast<DataTypePluginFactory*>(plugin) )
            {
            }
            else if ( qobject_cast<LoaderPluginFactory*>(plugin) )
            {
                action = gui.addMenuAction(plugin::Type::LOADER, name);
            }
            else if ( qobject_cast<TransformationPluginFactory*>(plugin) )
            {
                action = gui.addMenuAction(plugin::Type::TRANFORMATION, name);
            }
            else if ( qobject_cast<ViewPluginFactory*>(plugin) )
            {
                action = gui.addMenuAction(plugin::Type::VIEW, name);
            }
            else
            {
                qDebug() << "Plugin " << fileName << " does not implement any of the possible interfaces!";
                return;
            }
            
            if(action)
            {
                QObject::connect(action, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
                signalMapper->setMapping(action, _plugins.size()-1);
            }
        }
    }

    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &PluginManager::pluginTriggered);
}

void PluginManager::pluginTriggered(int idx)
{
    assert(idx >= 0 && idx < _plugins.size());
    
    PluginFactory *plugin = _plugins[idx];
    
    Plugin* instance = plugin->produce();
    qDebug() << "Added plugin" << instance->getName() << "with version" << instance->getVersion();
}

} // namespace plugin

} // namespace hdps
