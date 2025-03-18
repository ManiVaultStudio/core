// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginManager.h"

#include "Core.h"

#include <AnalysisPlugin.h>
#include <LoaderPlugin.h>
#include <PluginFactory.h>
#include <PluginType.h>
#include <RawData.h>
#include <TransformationPlugin.h>
#include <ViewPlugin.h>
#include <WriterPlugin.h>

#include <util/Serialization.h>

#include <actions/PluginTriggerAction.h>

#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QPluginLoader>
#include <QSet>

#include <cassert>
#include <utility>
#include <stdexcept>

#include "models/PluginsListModel.h"

#ifdef _DEBUG
    //#define PLUGIN_MANAGER_VERBOSE
#endif

namespace mv {

using namespace util;
using namespace plugin;
using namespace gui;

PluginManager::PluginManager(QObject* parent) :
    AbstractPluginManager(parent),
    _listModel(nullptr),
    _treeModel(nullptr)
{
    setObjectName("Plugins");
}

PluginManager::~PluginManager()
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
        loadPluginFactories();

        _listModel = new PluginsListModel(AbstractPluginsModel::PopulationMode::Automatic, this);
        _treeModel = new PluginsTreeModel(AbstractPluginsModel::PopulationMode::Automatic, this);
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
        QStringList pluginIds;

        for (const auto& plugin : _plugins)
            pluginIds << plugin->getId();

        for (const auto& pluginId : pluginIds)
            destroyPluginById(pluginId);
    }
    endReset();
}

void PluginManager::loadPluginFactories()
{
#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << "Loading plugin factories";
#endif

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

    auto getPluginDependencyDir = [](const QDir& dir, const QString& name) -> std::pair<QDir, bool> {
        QDir dependenciesDir = dir;
        dependenciesDir.cdUp();
        if (!dependenciesDir.cd("PluginDependencies"))  // name by convention
            return { dependenciesDir, false };
        if (!dependenciesDir.cd(name))
            return { dependenciesDir, false };
        dependenciesDir.cd(name);
        return { dependenciesDir, true };
        };

    auto getLibraryName = [](const QString& libFileName) -> QString {
        // Use QFileInfo to get the base name without the suffix (.dll, .so, .dylib)
        QFileInfo fileInfo(libFileName);
        QString baseName = fileInfo.baseName();

        // Check for common "lib" prefix on UNIX systems and remove it
        if (baseName.startsWith("lib")) {
            baseName = baseName.mid(3);  // Remove the first 3 characters ("lib")
        }

        return baseName;
        };

    auto loadPluginDependencies = [&getLibraryName, &getPluginDependencyDir](const QDir& pluginDir, const QString& fileName) -> void {

        const auto pluginName = getLibraryName(fileName);
        const auto [pluginDependenciesDir, pluginDependenciesExists] = getPluginDependencyDir(pluginDir, pluginName);

        if (!pluginDependenciesExists)
            return;

        QSet<QString> dynamicLibsToLoad;
        for (const QFileInfo& fileInfo : pluginDependenciesDir.entryInfoList({}, QDir::Files))
        {
            const auto filePath = fileInfo.absoluteFilePath();
            if (!QLibrary::isLibrary(filePath))
                continue;

            dynamicLibsToLoad.insert(filePath);
        }

        // Some dependencies might depend on other dynamic libraries
        // We could build a dependency tree, but this works just as well
        const size_t maxTries = dynamicLibsToLoad.size();
        size_t currentTry = 0;
        while (!dynamicLibsToLoad.isEmpty()) {
            for (const QString& dynamicLibPath : dynamicLibsToLoad) {
                QLibrary lib(dynamicLibPath);
                if (lib.load()) {
                    qDebug() << "Loaded dependency: " << dynamicLibPath;
                    dynamicLibsToLoad.remove(dynamicLibPath);
                }
            }

            if (dynamicLibsToLoad.isEmpty())
                break;

            if (++currentTry >= maxTries)
            {
                qWarning() << "Could not load all dependencies for " << pluginName << ". Missing: " << dynamicLibsToLoad;
                break;
            }
        }
        };

    // List of filenames of dependency resolved plugins
    const QStringList resolvedPlugins = resolveDependencies(pluginDir);

    // For each of the plugin files which are resolved, load them and add them in their proper menu category
    for (const auto& fileName: resolvedPlugins)
    {
        // Load plugin dependencies, if there are any
        loadPluginDependencies(pluginDir, fileName);

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
        _pluginFactories[pluginKind]->getPluginMetadata().getVersion().setContext(QString("%1 plugin").arg(pluginKind).toStdString());
        _pluginFactories[pluginKind]->getPluginMetadata().getVersion().initialize(version);
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

    emit pluginFactoriesLoaded();
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
#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << "Resolving plugin dependencies";
#endif

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

        if (pluginInstance == nullptr)
            throw std::runtime_error(QString("Unable to produce plugin of kind %1").arg(kind).toStdString());

        if (!pluginFactory->mayProduce())
            throw std::runtime_error("Maximum number of plugin instances reached");

        pluginFactory->setNumberOfInstances(pluginFactory->getNumberOfInstances() + 1);
        pluginFactory->setNumberOfInstancesProduced(pluginFactory->getNumberOfInstancesProduced() + 1);

        switch (pluginFactory->getType()) {
            case plugin::Type::ANALYSIS: {
                auto analysisPlugin = dynamic_cast<AnalysisPlugin*>(pluginInstance);

                if (!inputDatasets.isEmpty())
                    analysisPlugin->setInputDatasets(inputDatasets);

                if (!outputDatasets.isEmpty())
                    analysisPlugin->setOutputDatasets(outputDatasets);

                break;
            }

            case plugin::Type::TRANSFORMATION: {
                auto analysisPlugin = dynamic_cast<TransformationPlugin*>(pluginInstance);

                if (!inputDatasets.isEmpty())
                    analysisPlugin->setInputDatasets(inputDatasets);

                break;
            }

            case plugin::Type::WRITER: {
                auto writerPlugin = dynamic_cast<WriterPlugin*>(pluginInstance);

                if (!inputDatasets.isEmpty())
                    writerPlugin->setInputDatasets(inputDatasets);

                break;
            }

            default:
                break;
        }

        addPlugin(pluginInstance);

#ifdef PLUGIN_MANAGER_VERBOSE
        qDebug() << "Added plugin" << pluginInstance->getKind() << "with version" << pluginInstance->getVersion() << "and ID" << pluginInstance->getId();
#endif

        _plugins.push_back(std::move(std::unique_ptr<plugin::Plugin>(pluginInstance)));

        emit pluginAdded(pluginInstance);

        return pluginInstance;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create plugin");
    }

    return {};
}

plugin::ViewPlugin* PluginManager::requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, gui::DockAreaFlag dockArea /*= gui::DockAreaFlag::Right*/, Datasets datasets /*= Datasets()*/)
{
    try
    {
	    if (!_pluginFactories.keys().contains(kind))
	        throw std::runtime_error("Unrecognized view plugin kind");

        auto viewPluginFactory = dynamic_cast<plugin::ViewPluginFactory*>(_pluginFactories[kind]);

        if (!viewPluginFactory)
            throw std::runtime_error(QString("%1 plugin factory does not exist").arg(kind).toStdString());

	    if (viewPluginFactory->getStartFloating())
	        return requestViewPluginFloated(kind, datasets);

	    const auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(requestPlugin(kind, datasets));

	    if (viewPlugin != nullptr)
	        mv::workspaces().addViewPlugin(viewPlugin, dockToViewPlugin, dockArea);

	    return viewPlugin;
	}
	catch (std::exception& e)
	{
	    exceptionMessageBox("Unable to create view plugin", e);
	}
	catch (...) {
	    exceptionMessageBox("Unable to create view plugin");
	}

	return {};
}

plugin::ViewPlugin* PluginManager::requestViewPluginFloated(const QString& kind, Datasets datasets)
{
    const auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(requestPlugin(kind, datasets));

    if (viewPlugin != nullptr)
        mv::workspaces().addViewPluginFloated(viewPlugin);

    return viewPlugin;
}

void PluginManager::addPlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin != nullptr);

    if (plugin == nullptr)
        return;

#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << "Add plugin to manager" << plugin->getGuiName();
#endif

    try
    {
        switch (plugin->getType())
        {
            case plugin::Type::DATA:
            {
                mv::data().addRawData(dynamic_cast<plugin::RawData*>(plugin));
                break;
            }

            case plugin::Type::VIEW:
                break;
        }

        plugin->init();

        switch (plugin->getType())
        {
            case plugin::Type::ANALYSIS:
            {
                auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

                if (analysisPlugin)
                    events().notifyDatasetAdded(analysisPlugin->getOutputDataset());

                break;
            }

            case plugin::Type::LOADER:
            {
                dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
                break;
            }

            case plugin::Type::WRITER:
            {
                dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();
                break;
            }

        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add plugin");
    }
}

void PluginManager::destroyPlugin(plugin::Plugin* plugin)
{
    Q_ASSERT(plugin != nullptr);

    if (plugin == nullptr)
        return;

#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << "Destroy plugin" << plugin->getGuiName();
#endif

    try
    {
        const auto pluginId = plugin->getId();

        emit pluginAboutToBeDestroyed(plugin);
        {
            const auto it = std::find_if(_plugins.begin(), _plugins.end(), [pluginId](const auto& pluginPtr) -> bool {
                return pluginId == pluginPtr->getId();
            });

            if (it == _plugins.end())
                throw std::runtime_error(QString("Plugin %1 (%2) not found").arg(plugin->getGuiName(), plugin->getId()).toStdString());

            _plugins.erase(it);
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

void PluginManager::destroyPluginById(const QString& pluginId)
{
#ifdef PLUGIN_MANAGER_VERBOSE
    qDebug() << "Destroy plugin by identifier" << plugin->getGuiName();
#endif

    try
    {
        const auto it = std::find_if(_plugins.begin(), _plugins.end(), [pluginId](const auto& pluginPtr) -> bool {
            return pluginId == pluginPtr->getId();
		});

        if (it == _plugins.end())
            throw std::runtime_error(QString("Plugin (%1) not found").arg(pluginId).toStdString());

        auto plugin = it->get();

        emit pluginAboutToBeDestroyed(plugin);
        {
            _plugins.erase(it);
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

std::vector<PluginFactory*> PluginManager::getPluginFactoriesByType(const plugin::Type& pluginType) const
{
    std::vector<PluginFactory*> pluginFactories;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginFactories.push_back(pluginFactory);

    std::sort(pluginFactories.begin(), pluginFactories.end(), [](auto pluginFactoryLhs, auto pluginFactoryRhs) -> bool {
        return pluginFactoryLhs->getKind() < pluginFactoryRhs->getKind();
    });

    return pluginFactories;
}

std::vector<PluginFactory*> PluginManager::getPluginFactoriesByTypes(const plugin::Types& pluginTypes /*= plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }*/) const
{
    std::vector<PluginFactory*> pluginFactories;

    for (const auto& pluginType : pluginTypes) {
        auto pluginFactoriesForType = getPluginFactoriesByType(pluginType);

        std::sort(pluginFactoriesForType.begin(), pluginFactoriesForType.end(), [](auto pluginFactoryLhs, auto pluginFactoryRhs) -> bool {
            return pluginFactoryLhs->getKind() < pluginFactoryRhs->getKind();
        });

        pluginFactories.insert(pluginFactories.end(), pluginFactoriesForType.begin(), pluginFactoriesForType.end());
    }

    return pluginFactories;
}

std::vector<plugin::Plugin*> PluginManager::getPluginsByFactory(const plugin::PluginFactory* pluginFactory) const
{
    std::vector<plugin::Plugin*> pluginsByFactory;

    for (const auto& plugin : getPluginsByTypes())
        if (pluginFactory == plugin->getFactory())
            pluginsByFactory.push_back(plugin);

    std::sort(pluginsByFactory.begin(), pluginsByFactory.end(), [](auto pluginLhs, auto pluginRhs) -> bool {
        return pluginLhs->getKind() < pluginRhs->getKind();
    });

    return pluginsByFactory;
}

std::vector<plugin::Plugin*> PluginManager::getPluginsByType(const plugin::Type& pluginType) const
{
    std::vector<plugin::Plugin*> pluginsByType;

    for (const auto& plugin : const_cast<PluginManager*>(this)->_plugins)
        if (pluginType == plugin->getType())
            pluginsByType.push_back(plugin.get());

    std::sort(pluginsByType.begin(), pluginsByType.end(), [](auto pluginLhs, auto pluginRhs) -> bool {
        return pluginLhs->getKind() < pluginRhs->getKind();
    });

    return pluginsByType;
}

std::vector<plugin::Plugin*> PluginManager::getPluginsByTypes(const plugin::Types& pluginTypes /*= plugin::Types{ plugin::Type::ANALYSIS, plugin::Type::DATA, plugin::Type::LOADER, plugin::Type::WRITER, plugin::Type::TRANSFORMATION, plugin::Type::VIEW }*/) const
{
    std::vector<plugin::Plugin*> pluginsByType;

    for (const auto& pluginType : pluginTypes) {
        auto pluginsForType = getPluginsByType(pluginType);
        
        std::sort(pluginsForType.begin(), pluginsForType.end(), [](auto pluginLhs, auto pluginRhs) -> bool {
            return pluginLhs->getKind() < pluginRhs->getKind();
        });

        pluginsByType.insert(pluginsByType.end(), pluginsForType.begin(), pluginsForType.end());
    }

    return pluginsByType;
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

    PluginFactory::initializePluginTriggerActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const Type& pluginType, const Datasets& datasets) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(datasets);

    sortActions(pluginProducerActions);

    PluginFactory::initializePluginTriggerActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getType() == pluginType)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(dataTypes);

    sortActions(pluginProducerActions);

    PluginFactory::initializePluginTriggerActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories)
        if (pluginFactory->getKind() == pluginKind)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(datasets);

    sortActions(pluginProducerActions);

    PluginFactory::initializePluginTriggerActions(pluginProducerActions);

    return pluginProducerActions;
}

PluginTriggerActions PluginManager::getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const
{
    PluginTriggerActions pluginProducerActions;

    for (auto pluginFactory : _pluginFactories) 
        if (pluginFactory->getKind() == pluginKind)
            pluginProducerActions << pluginFactory->getPluginTriggerActions(dataTypes);

    sortActions(pluginProducerActions);

    PluginFactory::initializePluginTriggerActions(pluginProducerActions);

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
        return {};

    return StyledIcon(_pluginFactories[pluginKind]->icon());
}

void PluginManager::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "UsedPlugins");

    QStringList missingPluginKinds;

    for (const auto& usedPlugin : variantMap["UsedPlugins"].toList())
        if (!_pluginFactories.contains(usedPlugin.toString()))
            missingPluginKinds << usedPlugin.toString();

    if (variantMap.contains("LoadedAnalyses"))
    {
        for (const auto& loadedAnalysis : variantMap["LoadedAnalyses"].toList())
            missingPluginKinds.removeAll(loadedAnalysis.toMap()["Kind"].toString());

        missingPluginKinds.squeeze();

        if (!missingPluginKinds.isEmpty())
            throw std::runtime_error(QString("One or more plugins are not available: %1").arg(missingPluginKinds.join(", ")).toLocal8Bit());

        for (const auto& loadedAnalysis : variantMap["LoadedAnalyses"].toList())
        {
            auto analysisPluginMap = loadedAnalysis.toMap();

            variantMapMustContain(analysisPluginMap, "Kind");
            variantMapMustContain(analysisPluginMap, "InputDatasetsIDs");
            variantMapMustContain(analysisPluginMap, "OutputDatasetsIDs");

            auto analysisPluginKind = analysisPluginMap["Kind"].toString();

            if (_pluginFactories.contains(analysisPluginKind))
            {
                auto inputDatasetsGUIDs = analysisPluginMap["InputDatasetsIDs"].toStringList();
                auto outputDatasetsGUIDs = analysisPluginMap["OutputDatasetsIDs"].toStringList();

                Datasets inputDatasets;

                for (const auto& inputDatasetGUID : inputDatasetsGUIDs)
                    inputDatasets << mv::data().getDataset(inputDatasetGUID);

                Datasets outputDatasets;

                for (const auto& outputDatasetGUID : outputDatasetsGUIDs)
                    outputDatasets << mv::data().getDataset(outputDatasetGUID);

                auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugins().requestPlugin(analysisPluginKind, inputDatasets, outputDatasets));

                if (analysisPlugin)
                    analysisPlugin->fromVariantMap(analysisPluginMap);
            }
        }
    }
}

QVariantMap PluginManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList usedPluginsList;     // Kinds of used plugins
    QVariantList loadedAnalysesList;  // Opened analysis plugin instances

    for (const auto& pluginFactory : _pluginFactories.values())
        if ((pluginFactory->getType() == Type::DATA || pluginFactory->getType() == Type::ANALYSIS || pluginFactory->getType() == Type::VIEW) && pluginFactory->getNumberOfInstances() > 0)
            usedPluginsList << pluginFactory->getKind();

    for (const auto& loadedPlugin : _plugins) {
	    if (loadedPlugin->getType() == Type::ANALYSIS)
	    {
	    	// Make sure the analysisPlugin overloads toVariantMap() and fromVariantMap(QVariantMap) before saving it in the project
	    	auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(loadedPlugin.get());
	    	const QMetaObject* metaObj = analysisPlugin->metaObject();

	    	if (metaObj == nullptr)
	    		continue;

	    	auto toVariantMapIndex = metaObj->indexOfMethod(QMetaObject::normalizedSignature("toVariantMap()").constData());
	    	auto fromVariantMapIndex = metaObj->indexOfMethod(QMetaObject::normalizedSignature("fromVariantMap(QVariantMap)").constData());

	    	if (toVariantMapIndex != -1 && fromVariantMapIndex != -1)
	    		loadedAnalysesList << analysisPlugin->toVariantMap();
	    	else if (toVariantMapIndex != -1)
	    		qWarning() << "PluginManager::toVariantMap(): " << analysisPlugin->getName() << " implements toVariantMap() but not fromVariantMap(QVariantMap) - analysis plugin is not saved.";
	    	else if (fromVariantMapIndex != -1)
	    		qWarning() << "PluginManager::toVariantMap(): " << analysisPlugin->getName() << " implements fromVariantMap(QVariantMap) but not toVariantMap() - analysis plugin is not saved.";
	    }
    }

    variantMap.insert({
        { "UsedPlugins", usedPluginsList },
        { "LoadedAnalyses", loadedAnalysesList }
    });

    return variantMap;
}

const PluginsListModel& PluginManager::getListModel() const
{
    return *_listModel;
}

const PluginsTreeModel& PluginManager::getTreeModel() const
{
    return *_treeModel;
}

}
