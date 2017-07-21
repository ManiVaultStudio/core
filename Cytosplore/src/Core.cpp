#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "DataManager.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "DataTypePlugin.h"
#include "DataConsumer.h"

namespace hdps
{

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core()
{
    // Delete the plugin manager
    _pluginManager.reset();

    // Delete all plugins
    for (auto& kv : _plugins)
    {
        for (int i = 0; i < kv.second.size(); ++i)
        {
            kv.second[i].reset();
        }
    }
}

void Core::init()
{
    _pluginManager = std::make_unique<plugin::PluginManager>(*this);
    _dataManager = std::make_unique<DataManager>();

    _pluginManager->LoadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    plugin->setCore(this);

    // If it is a view plugin then it should be added to the main window
    if (plugin->getType() == plugin::Type::VIEW)
    {
        _mainWindow.addView(dynamic_cast<plugin::ViewPlugin*>(plugin));
    }

    // Add the plugin to a list of plugins of the same type
    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));

    // Initialize the plugin after it has been added to the core
    plugin->init();

    // If it is an analysis plugin with a settings widget, add the settings to the main window
    if (plugin->getType() == plugin::Type::ANALYSIS)
    {
        plugin::AnalysisPlugin* analysis = dynamic_cast<plugin::AnalysisPlugin*>(plugin);
        if (analysis->hasSettings())
        {
            _mainWindow.addSettings(analysis->getSettings());
        }
    }
    // If it is a loader plugin it should call loadData
    if (plugin->getType() == plugin::Type::LOADER)
    {
        dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
    }
    // If it is a writer plugin it should call writeData
    if (plugin->getType() == plugin::Type::WRITER)
    {
        dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();
    }
    // If the plugin is a data consumer, notify it about all the data present in the core
    plugin::DataConsumer* dataConsumer = dynamic_cast<plugin::DataConsumer*>(plugin);
    if (dataConsumer)
    {
        for (const Set* set : _dataManager->allSets())
        {
            dataConsumer->dataAdded(set->getName());
        }
    }
}

/**
 * Requests the plugin manager to create a new plugin of the given kind.
 * The manager will add the plugin instance to the core and return the
 * unique name of the plugin.
 */
const QString Core::addData(const QString kind, const QString name)
{
    QString pluginName = _pluginManager->AddPlugin(kind);
    const plugin::DataTypePlugin* dataType = dynamic_cast<plugin::DataTypePlugin*>(requestPlugin(pluginName));

    Set* fullSet = dataType->createSet();
    Set* selection = dataType->createSet();

    QString setName = _dataManager->getUniqueSetName(name);
    fullSet->setName(setName);
    fullSet->setDataName(pluginName);
    fullSet->setAll();
    selection->setDataName(pluginName);

    _dataManager->addSet(fullSet);
    _dataManager->addSelection(pluginName, selection);
    
    return setName;
}

/**
 * Creates a copy of the given selection set and gives it a unique name based
 * on the name given to this function. Then adds the new set to the data manager
 * and notifies all data consumers of the new set.
 */
void Core::createSubsetFromSelection(const Set* selection, const QString newSetName)
{
    Set* newSet = selection->copy();
    newSet->setName(_dataManager->getUniqueSetName(newSetName));

    _dataManager->addSet(newSet);
    notifyDataAdded(newSet->getName());
}

/**
 * Requests an instance of a data type plugin from the core which has the same
 * unique name as the given parameter. If no such instance can be found a fatal
 * error is thrown.
 */
plugin::DataTypePlugin* Core::requestPlugin(const QString name)
{
    for (std::unique_ptr<plugin::Plugin>& plugin : _plugins[plugin::Type::DATA_TYPE])
    {
        if (plugin->getName() == name)
        {
            return dynamic_cast<plugin::DataTypePlugin*>(plugin.get());
        }
    }
    qFatal((QString("Failed to find plugin with name: ") + name).toStdString().c_str());
}

Set* Core::requestData(const QString name)
{
    Set* set = _dataManager->getSet(name);
    return set;
}

Set* Core::requestSelection(const QString name)
{
    return _dataManager->getSelection(name);
}

/**
 * Goes through all plugins stored in the core and calls the 'dataAdded' function
 * on all plugins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsSet(dataConsumer, name))
            dataConsumer->dataAdded(name);
    }
}

/**
* Goes through all plugins stored in the core and calls the 'dataChanged' function
* on all plugins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsSet(dataConsumer, name))
            dataConsumer->dataChanged(name);
    }
}

/**
* Goes through all plugins stored in the core and calls the 'dataRemoved' function
* on all plugins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsSet(dataConsumer, name))
            dataConsumer->dataRemoved(name);
    }
}

/** Notify all data consumers that a selection has changed. */
void Core::notifySelectionChanged(const QString dataName)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        dataConsumer->selectionChanged(dataName);
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

bool Core::supportsSet(plugin::DataConsumer* dataConsumer, QString setName)
{
    const hdps::Set* set = requestData(setName);
    const plugin::DataTypePlugin* dataPlugin = requestPlugin(set->getDataName());

    return dataConsumer->supportedDataKinds().contains(dataPlugin->getKind());
}

std::vector<plugin::DataConsumer*> Core::getDataConsumers()
{
    std::vector<plugin::DataConsumer*> dataConsumers;
    for (auto& kv : _plugins)
    {
        for (int i = 0; i < kv.second.size(); ++i)
        {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc)
                dataConsumers.push_back(dc);
        }
    }
    return dataConsumers;
}

} // namespace hdps
