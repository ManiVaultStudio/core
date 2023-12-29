// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginManager.h"
#include "Core.h"

#include <PluginFactory.h>
#include <LoaderPlugin.h>
#include <WriterPlugin.h>
#include <ViewPlugin.h>
#include <AnalysisPlugin.h>
#include <TransformationPlugin.h>
#include <RawData.h>
#include <PluginType.h>

#include <util/Serialization.h>

#include <QDebug>
#include <QPluginLoader>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <stdexcept>
#include <assert.h>

#ifdef _DEBUG
    //#define PLUGIN_MANAGER_VERBOSE
#endif

namespace mv {

using namespace util;
using namespace plugin;

PluginManager::PluginManager() :
    AbstractPluginManager()
{
    setObjectName("Plugins");
}

PluginManager::~PluginManager(void)
{
    reset();
}

void PluginManager::initialize()
{
#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractPluginManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        loadPlugins();
    }
    endInitialization();
}

void PluginManager::reset()
{
#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        for (auto& pluginPtr : _plugins)
            destroyPlugin(pluginPtr);

        _plugins.clear();
    }
    endReset();
}

void PluginManager::loadPlugins()
{
	QDir pluginDir(qApp->applicationDirPath());
    
#if defined(Q_OS_WIN)
    //if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
    //    pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginDir.dirName() == "MacOS")
    {
        pluginDir.cdUp();
        pluginDir.cdUp();
        pluginDir.cdUp();
    }
#endif
    pluginDir.cd("Plugins");
    
    _pluginFactories.clear();

    // List of filenames of dependency resolved plugins
    const QStringList resolvedPlugins = resolveDependencies(pluginDir);

    // For each of the plugin files which are resolved, load them and add them in their proper menu category
    for (const QString& fileName: resolvedPlugins)
    {
        // Dynamic loader of plugin shared library
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));
        
        // Get metadata about plugin from the accompanying .json file compiled in the shared library
        QString pluginKind      = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();
        QString menuName        = pluginLoader.metaData().value("MetaData").toObject().value("menuName").toString();
        QString version         = pluginLoader.metaData().value("MetaData").toObject().value("version").toString();
        
        // Create an instance of the plugin, i.e. the factory
        auto pluginFactory = dynamic_cast<PluginFactory*>(pluginLoader.instance());

        // If pluginFactory is a nullptr then loading of the plugin failed for some reason. Print the reason to output.
        if (!pluginFactory)
        {
            qWarning() << "Failed to load plugin: " << fileName << pluginLoader.errorString();
            continue;
        }

        // Loading of the plugin succeeded so cast it to its original class
        _pluginFactories[pluginKind] = pluginFactory;
        _pluginFactories[pluginKind]->setKind(pluginKind);
        _pluginFactories[pluginKind]->setVersion(version);
        _pluginFactories[pluginKind]->initialize();

        if (qobject_cast<AnalysisPluginFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<RawDataFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<LoaderPluginFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<WriterPluginFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<ViewPluginFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<TransformationPluginFactory*>(pluginFactory))
        {
        }
        else
        {
            qDebug() << "Plugin " << fileName << " does not implement any of the possible interfaces!";
            return;
        }
    }
}

bool PluginManager::isPluginLoaded(const QString& kind) const
{
    return _pluginFactories.keys().contains(kind);
}

mv::plugin::PluginFactory* PluginManager::getPluginFactory(const QString& pluginKind) const
{
    if (!isPluginLoaded(pluginKind))
        return nullptr;

    return _pluginFactories[pluginKind];
}

QStringList PluginManager::resolveDependencies(QDir pluginDir) const
{
    // Map keeping track of the list of plugin kinds on which a plugin is dependent
    QMap<QString, QStringList> dependencies;
    // List of plugin kinds which have had their dependencies resolved
    QStringList resolved;
    // Map from plugin kinds to plugin file names
    QMap<QString, QString> kindToPluginNameMap;

    /*
     * For each item in the plugin directory, store their dependencies. If a plugin has no dependencies,
     * immediately add it to the list of resolved plugins. Dependencies are given by a list of plugin kinds
     * under the 'dependencies' key in the accompanying .json metadata file.
     */
    for (QString fileName: pluginDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));

        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();
        QString kind = metaData.value("name").toString();
        QJsonArray dependencyData = metaData.value("dependencies").toArray();

        // Map plugin kind to plugin file name
        kindToPluginNameMap[kind] = fileName;

        // If plugin has no dependencies, add it to the resolved list
        if (dependencyData.size() == 0)
        {
            resolved.push_back(kind);
            continue;
        }

        // Store plugin dependency list in a map
        QStringList dependencyList;
        for (const QJsonValue& dependency : dependencyData)
        {
            dependencyList.push_back(dependency.toString());
        }
        dependencies[kind] = dependencyList;
    }

    qDebug() << "Dependencies: " << dependencies;
    qDebug() << "Resolved: " << resolved;

    // Attempt to resolve all plugin dependencies in an iterative manner
    while (dependencies.size() != 0)
    {
        bool removed = false;

        // For all dependencies of a plugin, check if any of its dependencies are already resolved.
        // If so, remove that dependency from the list.
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

        // Check for unresolved plugins which now have zero dependencies and mark them as resolved
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

    // TODO List all of the plugins which have unresolved dependencies and what they are
    // Check if this plugin has all its dependencies resolved
    /*if (!resolvedPlugins.contains(fileName)) {
        qWarning() << "Plugin: " << pluginName << " has unresolved dependencies.";
        continue;
    }*/

    // Form a list of resolved plugin file names rather than kinds and return it
    QStringList resolvedOrderedPluginNames;
    for (const QString& kind : resolved)
    {
        resolvedOrderedPluginNames.push_back(kindToPluginNameMap[kind]);
    }
    return resolvedOrderedPluginNames;
}

plugin::Plugin* PluginManager::requestPlugin(const QString& kind, Datasets inputDatasets /*= Datasets()*/, Datasets outputDatasets /*= Datasets()*/)
{
    try
    {
        if (!_pluginFactories.keys().contains(kind))
            throw std::runtime_error("Unrecognized plugin kind");

        auto pluginFactory  = _pluginFactories[kind];
        auto pluginInstance = pluginFactory->produce();

        if (!pluginInstance)
            return nullptr;

        pluginFactory->setNumberOfInstances(pluginFactory->getNumberOfInstances() + 1);

        switch (pluginFactory->getType()) {
            case plugin::Type::ANALYSIS: {
                auto analysisPlugin = dynamic_cast<AnalysisPlugin*>(pluginInstance);

                if (!inputDatasets.isEmpty())
                    analysisPlugin->setInputDataset(inputDatasets.first());

                if (!outputDatasets.isEmpty())
                    analysisPlugin->setOutputDataset(outputDatasets.first());

                break;
            }
            case plugin::Type::WRITER: {
                auto writerPlugin = dynamic_cast<WriterPlugin*>(pluginInstance);

                if (!inputDatasets.isEmpty())
                    writerPlugin->setInputDataset(inputDatasets.first());

                break;
            }

            default:
                break;
        }

        dynamic_cast<Core*>(Application::core())->addPlugin(pluginInstance);

        qDebug() << "Added plugin" << pluginInstance->getKind() << "with version" << pluginInstance->getVersion();

        _plugins << pluginInstance;

        emit pluginAdded(pluginInstance);

        return pluginInstance;
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, "HDPS", QString("Unable to create plugin: %1").arg(e.what()));

        return nullptr;
    }
}

mv::plugin::ViewPlugin* PluginManager::requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, gui::DockAreaFlag dockArea /*= gui::DockAreaFlag::Right*/, Datasets datasets /*= Datasets()*/)
{
    auto viewPlugin = dynamic_cast<mv::plugin::ViewPlugin*>(requestPlugin(kind, datasets));

    if (viewPlugin)
        Application::core()->getWorkspaceManager().addViewPlugin(viewPlugin, dockToViewPlugin, dockArea);

    return viewPlugin;
}

void PluginManager::destroyPlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin != nullptr);

#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << plugin->getGuiName();
#endif

    try
    {
        const auto pluginId = plugin->getId();

        emit pluginAboutToBeDestroyed(plugin);
        {
            auto pluginFactory = _pluginFactories[plugin->getKind()];

            pluginFactory->setNumberOfInstances(pluginFactory->getNumberOfInstances() - 1);

            _plugins.removeOne(plugin);

            delete plugin;
        }
        emit pluginDestroyed(pluginId);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to destroy plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to destroy plugin");
    }
}

PluginFactoryPtrs PluginManager::getPluginFactoriesByType(const plugin::Type& pluginType) const
{
    PluginFactoryPtrs pluginFactories;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginFactories.push_back(pluginFactory);

    return pluginFactories;
}

PluginFactoryPtrs PluginManager::getPluginFactoriesByTypes(const plugin::Types& pluginTypes /*= plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }*/) const
{
    PluginFactoryPtrs pluginFactories;

    for (auto pluginType : pluginTypes) {
        const auto pluginFactoriesForType = getPluginFactoriesByType(pluginType);

        pluginFactories.insert(pluginFactories.end(), pluginFactoriesForType.begin(), pluginFactoriesForType.end());
    }

    return pluginFactories;
}

PluginPtrs PluginManager::getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const
{
    PluginPtrs plugins;

    for (auto& plugin : getPluginsByTypes())
        if (pluginFactory == plugin->getFactory())
            plugins.push_back(plugin);

    return plugins;
}

PluginPtrs PluginManager::getPluginsByType(const plugin::Type& pluginType) const
{
    PluginPtrs plugins;

    for (auto& plugin : const_cast<PluginManager*>(this)->_plugins)
        if (pluginType == plugin->getType())
            plugins.push_back(plugin);

    return plugins;
}

PluginPtrs PluginManager::getPluginsByTypes(const plugin::Types& pluginTypes /*= plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }*/) const
{
    PluginPtrs plugins;

    for (auto pluginType : pluginTypes) {
        const auto pluginsForType = getPluginsByType(pluginType);

        plugins.insert(plugins.end(), pluginsForType.begin(), pluginsForType.end());
    }
        
    return plugins;
}

QStringList PluginManager::getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const
{
    QStringList pluginKinds;

    for (const auto& pluginType : pluginTypes)
        for (auto pluginFactory : _pluginFactories)
            if (pluginFactory->getType() == pluginType)
                pluginKinds << pluginFactory->getKind();

    return pluginKinds;
}

mv::gui::PluginTriggerActions PluginManager::getPluginTriggerActions(const plugin::Type& pluginType) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginProducerActions << &pluginFactory->getPluginTriggerAction();

    sortActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const Type& pluginType, const Datasets& datasets) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(datasets);

    sortActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(dataTypes);

    sortActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getKind() == pluginKind)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(datasets);

    sortActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories) 
        if (pluginFactory->getKind() == pluginKind)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(dataTypes);

    sortActions(pluginProducerActions);

    return pluginProducerActions;
}

QString PluginManager::getPluginGuiName(const QString& pluginKind) const
{
    if (!_pluginFactories.contains(pluginKind))
        return "";

    return _pluginFactories[pluginKind]->getGuiName();
}

QIcon PluginManager::getPluginIcon(const QString& pluginKind) const
{
    if (!_pluginFactories.contains(pluginKind))
        return QIcon();

    return _pluginFactories[pluginKind]->getIcon();
}

void PluginManager::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "UsedPlugins");

    QStringList missingPluginKinds;

    for (const auto& usedPlugin : variantMap["UsedPlugins"].toList())
        if (!_pluginFactories.contains(usedPlugin.toString()))
            missingPluginKinds << usedPlugin.toString();

    if (!missingPluginKinds.isEmpty())
        throw std::runtime_error(QString("One or more plugins are not available: %1").arg(missingPluginKinds.join(", ")).toLocal8Bit());
}

QVariantMap PluginManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList usedPluginsList;

    for (auto pluginFactory : _pluginFactories.values())
        if ((pluginFactory->getType() == Type::DATA || pluginFactory->getType() == Type::ANALYSIS || pluginFactory->getType() == Type::VIEW) && pluginFactory->getNumberOfInstances() > 0)
            usedPluginsList << pluginFactory->getKind();

    variantMap.insert({
        { "UsedPlugins", usedPluginsList }
    });

    return variantMap;
}

}
