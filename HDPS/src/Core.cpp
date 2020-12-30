#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "exceptions/SetNotFoundException.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "RawData.h"
#include "Set.h"

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
        case plugin::Type::DATA:
        {
            _dataManager->addRawData(dynamic_cast<RawData*>(plugin));
            break;
        }

        // If it is a view plugin then it should be added to the main window
        case plugin::Type::VIEW:
        {
            _mainWindow.addPlugin(plugin);
            // fallthrough
        }

        // Otherwise add the plugin to a list of plug-ins of the same type
        default:
        {
            _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));
        }
    }
    
    // Initialize the plugin after it has been added to the core
    plugin->init();

    // If it is an analysis plugin with a settings widget, add the settings to the main window
    if (plugin->getType() == plugin::Type::ANALYSIS)
    {
        plugin::AnalysisPlugin* analysis = dynamic_cast<plugin::AnalysisPlugin*>(plugin);
        if (analysis->hasSettings())
        {
            _mainWindow.addPlugin(analysis);
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
}

const QString Core::addData(const QString kind, const QString nameRequest)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);
    // Request it from the core
    const RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    DataSet* fullSet = rawData.createDataSet();
    DataSet* selection = rawData.createDataSet();

    // Set the properties of the new sets
    fullSet->setAll(true);

    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);
    _dataManager->addSelection(rawDataName, selection);
    
    return setName;
}

void Core::removeDataset(const QString datasetName)
{
    _dataManager->removeDataset(datasetName);

    notifyDataRemoved(datasetName);
}

const QString Core::createDerivedData(const QString nameRequest, const QString sourceDatasetName)
{
    const DataSet& sourceSet = requestData(sourceDatasetName);
    DataType dataType = sourceSet.getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(dataType._type);
    // Request it from the core
    RawData& rawData = requestRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    DataSet* fullSet = rawData.createDataSet();
    fullSet->_sourceSetName = sourceDatasetName;
    fullSet->_derived = true;

    // Set properties of the new set
    fullSet->setAll(true);
    
    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);

    return setName;
}

QString Core::createSubsetFromSelection(const DataSet& selection, const DataSet& parentSet, const QString nameRequest)
{
    // Create a new set with only the indices that were part of the selection set
    DataSet* newSet = selection.copy();

    newSet->_dataName = parentSet._dataName;
    newSet->_sourceSetName = parentSet._sourceSetName;
    newSet->_derived = parentSet._derived;

    // Add the set the core and publish the name of the set to all plug-ins
    QString setName = _dataManager->addSet(nameRequest, newSet);
    notifyDataAdded(setName);

    return setName;
}

RawData& Core::requestRawData(const QString name)
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

const std::vector<const DataSet*> Core::requestDatasets()
{
    const auto& datasetMap = _dataManager->allSets();

    std::vector<const DataSet*> datasets(datasetMap.size());

    int i = 0;
    for (auto it = datasetMap.begin(); it != datasetMap.end(); ++it, i++) {
        datasets[i] = it->second.get();
    }
    return datasets;
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

// Utility function for inserting value into a map with std::vector values
// and creating the key and the empty std::vector if it didnt exist yet.
template <typename K, typename V>
void insertIntoVectorMap(std::unordered_map<K, std::vector<V>>& map, K k, V v)
{
    if (map.find(k) != map.end())
        map[k].push_back(v);
    else
    {
        std::vector<V> vec{ v };
        map[k] = vec;
    }
}

void Core::registerDatasetChanged(QString datasetName, DataChangedFunction func)
{
    insertIntoVectorMap<QString, DataChangedFunction>(datasetChangedListeners, datasetName, func);
}

void Core::registerSelectionChanged(QString datasetName, SelectionChangedFunction func)
{
    insertIntoVectorMap<QString, SelectionChangedFunction>(selectionChangedListeners, datasetName, func);
}

void Core::registerDataTypeAdded(DataType dataType, DataAddedFunction func)
{
    insertIntoVectorMap<DataType, DataAddedFunction>(dataTypeAddedListeners, dataType, func);
}

void Core::registerDataTypeChanged(DataType dataType, DataChangedFunction func)
{
    insertIntoVectorMap<DataType, DataChangedFunction>(dataTypeChangedListeners, dataType, func);
}

void Core::registerDataTypeRemoved(DataType dataType, DataRemovedFunction func)
{
    insertIntoVectorMap<DataType, DataRemovedFunction>(dataTypeRemovedListeners, dataType, func);
}

/**
 * Goes through all plug-ins stored in the core and calls the 'dataAdded' function
 * on all plug-ins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString name)
{
    DataType dt = requestData(name).getDataType();

    for (DataAddedFunction& callback : dataTypeAddedListeners[dt])
        callback(name);
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataChanged' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString name)
{
    DataType dt = requestData(name).getDataType();

    for (DataChangedFunction& callback : datasetChangedListeners[name])
        callback(name);
    for (DataChangedFunction& callback : dataTypeChangedListeners[dt])
        callback(name);
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataRemoved' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString name)
{
    DataType dt = requestData(name).getDataType();

    for (DataRemovedFunction& callback : dataTypeRemovedListeners[dt])
        callback(name);
}

/** Notify all data consumers that a selection has changed. */
void Core::notifySelectionChanged(const QString datasetName)
{
    for (SelectionChangedFunction& callback : selectionChangedListeners[datasetName])
        callback(datasetName);
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
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
