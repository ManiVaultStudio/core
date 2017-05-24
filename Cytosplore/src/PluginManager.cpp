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
#include "WriterPlugin.h"
#include "TransformationPlugin.h"
#include "ViewPlugin.h"
#include "PluginType.h"

namespace hdps {

namespace plugin {

PluginManager::PluginManager(Core& core)
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
    
    _pluginFactories.clear();
    
    QSignalMapper* signalMapper = new QSignalMapper(this);
    // for all items in the plugins directory
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        
        gui::MainWindow& gui = _core.gui();

        // create an instance of the plugin, i.e. the factory
        QObject *pluginFactory = pluginLoader.instance();
        if (pluginFactory)
        {
            QString kind = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();
            _pluginFactories[kind] = qobject_cast<PluginFactory*>(pluginFactory);

            QAction* action = NULL;

            if (qobject_cast<AnalysisPluginFactory*>(pluginFactory))
            {
                action = gui.addMenuAction(plugin::Type::ANALYSIS, kind);
            }
            else if (qobject_cast<DataTypePluginFactory*>(pluginFactory))
            {
            }
            else if (qobject_cast<LoaderPluginFactory*>(pluginFactory))
            {
                action = gui.addMenuAction(plugin::Type::LOADER, kind);
            }
            else if (qobject_cast<WriterPluginFactory*>(pluginFactory))
            {
                action = gui.addMenuAction(plugin::Type::WRITER, kind);
            }
            else if (qobject_cast<TransformationPluginFactory*>(pluginFactory))
            {
                action = gui.addMenuAction(plugin::Type::TRANFORMATION, kind);
            }
            else if (qobject_cast<ViewPluginFactory*>(pluginFactory))
            {
                action = gui.addMenuAction(plugin::Type::VIEW, kind);
            }
            else
            {
                qDebug() << "Plugin " << fileName << " does not implement any of the possible interfaces!";
                return;
            }
            
            if(action)
            {
                QObject::connect(action, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
                signalMapper->setMapping(action, kind);
            }
        }
    }

    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &PluginManager::pluginTriggered);
}

QString PluginManager::AddPlugin(const QString kind)
{
    return pluginTriggered(kind);
}

QString PluginManager::pluginTriggered(const QString& kind)
{
    PluginFactory *pluginFactory = _pluginFactories[kind];
    Plugin* plugin = pluginFactory->produce();

    _core.addPlugin(plugin);
    qDebug() << "Added plugin" << plugin->getKind() << "with version" << plugin->getVersion();
    return plugin->getName();
}

} // namespace plugin

} // namespace hdps
