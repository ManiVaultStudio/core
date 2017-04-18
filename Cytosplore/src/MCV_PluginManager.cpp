#include "MCV_PluginManager.h"

#include <QApplication>
#include <QMenuBar>
#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QSignalMapper>

#include "ui_MainWindow.h"

#include <assert.h>

#include "MCV_AnalysisPlugin.h"
#include "MCV_DataTypePlugin.h"
#include "MCV_LoaderPlugin.h"
#include "MCV_TransformationPlugin.h"
#include "MCV_ViewPlugin.h"

MCV_PluginManager::MCV_PluginManager(void)
{
    
}

MCV_PluginManager::~MCV_PluginManager(void)
{
    
}

void MCV_PluginManager::LoadPlugins(QSharedPointer<Ui::MainWindow> ui)
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
        
        // create an instance of the plugin, i.e. the factory
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            _plugins.push_back(plugin);
            QAction* action = NULL;
            
            QString name = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();
            
            if ( qobject_cast<MCV_AnalysisPluginFactory*>(plugin) )
            {
                action = ui->menuTransformation->addAction(name);
            }
            else if ( qobject_cast<MCV_DataTypePluginFactory*>(plugin) )
            {
            }
            else if ( qobject_cast<MCV_LoaderPluginFactory*>(plugin) )
            {
                action = ui->menuFile->addAction(name);
            }
            else if ( qobject_cast<MCV_TransformationPluginFactory*>(plugin) )
            {
                action = ui->menuTransformation->addAction(name);
            }
            else if ( qobject_cast<MCV_ViewPluginFactory*>(plugin) )
            {
                action = ui->menuVisualization->addAction(name);
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

    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &MCV_PluginManager::pluginTriggered);
}

void MCV_PluginManager::pluginTriggered(int idx)
{
    assert(idx >= 0 && idx < _plugins.size());
    
    QObject *plugin = _plugins[idx];
    
    if ( qobject_cast<MCV_AnalysisPluginFactory*>(plugin) )
    {
    } else if ( qobject_cast<MCV_DataTypePluginFactory*>(plugin) )
    {
    } else if ( qobject_cast<MCV_LoaderPluginFactory*>(plugin) )
    {
    } else if ( qobject_cast<MCV_TransformationPluginFactory*>(plugin) )
    {
    } else if ( qobject_cast<MCV_ViewPluginFactory*>(plugin) )
    {
        QSharedPointer<MCV_ViewPlugin> instance = qobject_cast<MCV_ViewPluginFactory*>(plugin)->produce();
        //test
        instance->dataRefreshed();
    }
}
