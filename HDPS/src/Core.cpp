#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "DataManager.h"

#include "exceptions/SetNotFoundException.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "RawData.h"
#include "Set.h"
#include "DataConsumer.h"

#include <QMessageBox>

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

    destroyPlugins();
}

void Core::init()
{
    _pluginManager = std::make_unique<plugin::PluginManager>(*this);
    _dataManager = std::make_unique<DataManager>();

    _pluginManager->loadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    plugin->setCore(this);

    switch (plugin->getType())
    {
        // If the plugin is RawData, then add it to the data manager
        case plugin::Type::DATA: _dataManager->addRawData(dynamic_cast<plugin::RawData*>(plugin)); break;

        // If it is a view plugin then it should be added to the main window
        case plugin::Type::VIEW: _mainWindow.addView(dynamic_cast<plugin::ViewPlugin*>(plugin)); // fallthrough

        // Otherwise add the plugin to a list of plug-ins of the same type
        default: _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));
    }
    
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
        try
        {
            dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
        }
        catch (plugin::DataLoadException e)
        {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", e.what());
            messageBox.setFixedSize(500, 200);
        }
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
        for (const auto& pair : _dataManager->allSets())
        {
            const DataSet& set = *pair.second;
            if (supportsDataSet(dataConsumer, set.getName()))
                dataConsumer->dataAdded(set.getName());
        }
    }
}

const QString Core::addData(const QString kind, const QString nameRequest)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);
    // Request it from the core
    const plugin::RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    DataSet* fullSet = rawData.createDataSet();
    DataSet* selection = rawData.createDataSet();

    // Set the properties of the new sets
    fullSet->setAll();

    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);
    _dataManager->addSelection(rawDataName, selection);
    
    return setName;
}

void Core::removeData(const QString dataName)
{
    QStringList removedSets = _dataManager->removeRawData(dataName);

    for (const QString& name : removedSets)
    {
        notifyDataRemoved(name);
    }
}

const QString Core::createDerivedData(const QString kind, const QString nameRequest, const QString sourceSetName)
{
    const DataSet& sourceSet = requestData(sourceSetName);

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(kind);
    // Request it from the core
    plugin::RawData& derivedData = requestRawData(pluginName);

    derivedData.setDerived(true, sourceSet.getDataName());

    // Create an initial full set, but no selection because it is shared with the source data
    DataSet* fullSet = derivedData.createDataSet();
    fullSet->_sourceSetName = sourceSetName;
    fullSet->_derived = true;

    // Set properties of the new set
    fullSet->setAll();
    
    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);

    return setName;
}

void Core::createSubsetFromSelection(const DataSet& selection, const QString dataName, const QString nameRequest)
{
    // Create a new set with only the indices that were part of the selection set
    DataSet* newSet = selection.copy();

    newSet->_dataName = dataName;

    // Add the set the core and publish the name of the set to all plug-ins
    QString setName = _dataManager->addSet(nameRequest, newSet);
    notifyDataAdded(setName);
}

plugin::RawData& Core::requestRawData(const QString name)
{
    try
    {
        return _dataManager->getRawData(name);
    }
    catch (DataNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

DataSet& Core::requestData(const QString name)
{
    try
    {
        return _dataManager->getSet(name);
    } 
    catch (SetNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

DataSet& Core::requestSelection(const QString name)
{
    try
    {
        return _dataManager->getSelection(name);
    }
    catch (const SelectionNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

/**
 * Goes through all plug-ins stored in the core and calls the 'dataAdded' function
 * on all plug-ins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsDataSet(dataConsumer, name))
            dataConsumer->dataAdded(name);
    }
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataChanged' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
        if (supportsDataSet(dataConsumer, name))
            dataConsumer->dataChanged(name);
    }
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataRemoved' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString name)
{
    for (plugin::DataConsumer* dataConsumer : getDataConsumers())
    {
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

/** Checks if the given data consumer supports the kind data in the given set. */
bool Core::supportsDataSet(plugin::DataConsumer* dataConsumer, QString setName)
{
    const hdps::DataSet& set = requestData(setName);
    const plugin::RawData& rawData = requestRawData(set.getDataName());

    return dataConsumer->supportedDataKinds().contains(rawData.getKind());
}

/** Retrieves all data consumers from the plug-in list. */
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

/** Destroys all plug-ins kept by the core */
void Core::destroyPlugins()
{
    for (auto& kv : _plugins)
    {
        for (int i = 0; i < kv.second.size(); ++i)
        {
            kv.second[i].reset();
        }
    }
}

} // namespace hdps
