#include "PluginManager.h"

#include <QApplication>
#include <QMenuBar>
#include <QDebug>
#include <QPluginLoader>
#include <QSignalMapper>

#include <assert.h>

#include "MainWindow.h"
#include "AnalysisPlugin.h"
#include "RawData.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "TransformationPlugin.h"
#include "ViewPlugin.h"
#include "PluginType.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

namespace hdps {

namespace plugin {

PluginManager::PluginManager(Core& core)
: _core(core)
{
    
}

PluginManager::~PluginManager(void)
{
    
}

void PluginManager::loadPlugins()
{
	QDir pluginDir(qApp->applicationDirPath());
    
#if defined(Q_OS_WIN)
    //if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
    //    pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS")
    {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginDir.cd("Plugins");
    
    _pluginFactories.clear();

    QSignalMapper* signalMapper = new QSignalMapper(this);
    
    QVector<QString> resolved = resolveDependencies(pluginDir);

    foreach(QString fileName, pluginDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));
        gui::MainWindow& gui = _core.gui();

        QString kind = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();

        // Check if this plugin has all its dependencies resolved
        if (!resolved.contains(kind)) {
            qWarning() << "Plugin: " << kind << " has unresolved dependencies.";
            continue;
        }

        // create an instance of the plugin, i.e. the factory
        QObject *pluginFactory = pluginLoader.instance();
       
        if (!pluginFactory)
        {
            qDebug() << "Failed to load plugin: " << fileName << pluginLoader.errorString();
            continue;
        }


        _pluginFactories[kind] = qobject_cast<PluginFactory*>(pluginFactory);

        QAction* action = NULL;

        if (qobject_cast<AnalysisPluginFactory*>(pluginFactory))
        {
            action = gui.addMenuAction(plugin::Type::ANALYSIS, kind);
        }
        else if (qobject_cast<RawDataFactory*>(pluginFactory))
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

    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &PluginManager::pluginTriggered);
}

QVector<QString> PluginManager::resolveDependencies(QDir pluginDir) const
{
    QMap<QString, QStringList> dependencies;
    QVector<QString> resolved;

    // for all items in the plugins directory
    foreach(QString fileName, pluginDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));

        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();
        QString kind = metaData.value("name").toString();
        QJsonArray dependencyData = metaData.value("dependencies").toArray();

        if (dependencyData.size() == 0)
        {
            resolved.push_back(kind);
            continue;
        }

        QStringList dependencyList;
        for (QJsonValue dependency : dependencyData)
        {
            dependencyList.push_back(dependency.toString());
        }
        dependencies[kind] = dependencyList;
    }
    qDebug() << "Dependencies: " << dependencies;
    qDebug() << "Resolved: " << resolved;
    // Dependency resolution
    while (dependencies.size() != 0)
    {
        bool removed = false;
        for (QStringList& depList : dependencies)
        {
            QMutableListIterator<QString> it(depList);
            while (it.hasNext()) {
                QString dependency = it.next();
                if (resolved.contains(dependency))
                {
                    it.remove();
                    removed = true;
                }
            }
        }
        QStringList keys = dependencies.keys();
        for (QString key : keys) {
            QStringList depList = dependencies[key];

            if (depList.size() == 0) {
                dependencies.remove(key);
                resolved.push_back(key);
                qDebug() << "Resolved: " << key;
            }
        }
        // If no dependencies were resolved this iteration we can stop
        if (!removed)
            break;
    }

    return resolved;
}

QString PluginManager::createPlugin(const QString kind)
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
