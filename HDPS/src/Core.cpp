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

#include "DataAnalysisAction.h"
#include "DataExportAction.h"

#include <QMessageBox>
#include <algorithm>

using namespace hdps::util;

namespace hdps
{

Core::Core(gui::MainWindow& mainWindow) :
    _mainWindow(mainWindow),
    _dataHierarchyManager(this)
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
    _dataManager = std::make_unique<DataManager>(this);

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
            _dataManager->addRawData(dynamic_cast<plugin::RawData*>(plugin));
            break;
        }

        // If it is a view plugin then it should be added to the main window
        case plugin::Type::VIEW:
        {
            _mainWindow.addPlugin(plugin);
        }

        // Otherwise add the plugin to a list of plug-ins of the same type
        default:
        {
            _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));
        }
    }
    
    // Initialize the plugin after it has been added to the core
    plugin->init();

    switch (plugin->getType())
    {
        case plugin::Type::ANALYSIS:
        {
            auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

            _mainWindow.addPlugin(plugin);
            _dataHierarchyManager.getHierarchyItem(analysisPlugin->getOutputDatasetName())->addIcon("analysis", analysisPlugin->getIcon());

            notifyDataAdded(analysisPlugin->getOutputDatasetName());

            break;
        }

        case plugin::Type::LOADER:
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

            break;
        }

        case plugin::Type::WRITER:
        {
            dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();

            break;
        }

    }
}

const QString Core::addData(const QString kind, const QString nameRequest, const QString& parentDatasetName /*= ""*/)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);

    // Request it from the core
    const plugin::RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet    = rawData.createDataSet();
    auto selection  = rawData.createDataSet();

    // Set the properties of the new sets
    fullSet->setAll(true);

    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);
    
    _dataManager->addSelection(rawDataName, selection);
    
    // Add the dataset to the hierarchy manager and select the dataset
    _dataHierarchyManager.addDataset(setName, parentDatasetName);
    _dataHierarchyManager.selectHierarchyItem(setName);
    
    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    new DataAnalysisAction(&_mainWindow, this, getDataHierarchyItem(setName));
    new DataExportAction(&_mainWindow, this, getDataHierarchyItem(setName));

    return setName;
}

void Core::removeDataset(const QString datasetName)
{
    _dataManager->removeDataset(datasetName);

    notifyDataRemoved(datasetName);
}

QString Core::renameDataset(const QString& currentDatasetName, const QString& intendedDatasetName)
{
    return _dataManager->renameSet(currentDatasetName, intendedDatasetName);
}

const QString Core::createDerivedData(const QString& nameRequest, const QString& sourceDatasetName, const QString& dataHierarchyParent /*= ""*/)
{
    const DataSet& sourceSet = requestData(sourceDatasetName);

    // Get the data type
    const auto dataType = sourceSet.getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(dataType._type);

    // Request it from the core
    plugin::RawData& rawData = requestRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    auto fullSet = rawData.createDataSet();

    fullSet->_sourceSetName = sourceDatasetName;
    fullSet->_derived = true;

    // Set properties of the new set
    fullSet->setAll(true);
    
    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager.addDataset(setName, dataHierarchyParent.isEmpty() ? sourceDatasetName : dataHierarchyParent);

    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    new DataAnalysisAction(&_mainWindow, this, getDataHierarchyItem(setName));
    new DataExportAction(&_mainWindow, this, getDataHierarchyItem(setName));

    return setName;
}

QString Core::createSubsetFromSelection(const DataSet& selection, const DataSet& sourceSet, const QString newSetName, const QString dataHierarchyParent /*= ""*/, const bool& visible /*= true*/)
{
    // Create a new set with only the indices that were part of the selection set
    auto newSet = selection.copy();

    newSet->_dataName       = sourceSet._dataName;
    newSet->_sourceSetName  = sourceSet._sourceSetName;
    newSet->_derived        = sourceSet._derived;

    // Add the set the core and publish the name of the set to all plug-ins
    const auto setName = _dataManager->addSet(newSetName, newSet);
    
    notifyDataAdded(setName);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager.addDataset(setName, dataHierarchyParent.isEmpty() ? sourceSet.getName() : dataHierarchyParent, visible);

    // Initialize the dataset (e.g. setup default actions for info)
    newSet->init();

    new DataAnalysisAction(&_mainWindow, this, getDataHierarchyItem(setName));
    new DataExportAction(&_mainWindow, this, getDataHierarchyItem(setName));

    return setName;
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

hdps::plugin::Plugin& Core::requestAnalysis(const QString name)
{
    try {
        auto analysisPluginsMap = std::map<QString, hdps::plugin::Plugin*>();

        for (const auto& analysisPlugin : _plugins[plugin::Type::ANALYSIS]) {
            analysisPluginsMap[analysisPlugin->getName()] = analysisPlugin.get();
        }

        return *analysisPluginsMap[name];
    }
    catch (AnalysisNotFoundException e)
    {
        QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
    }
}

std::vector<QString> Core::requestAllDataNames()
{
    const auto& datasetMap = _dataManager->allSets();

    std::vector<QString> datasetNames(datasetMap.size());

    int i = 0;
    for (auto it = datasetMap.begin(); it != datasetMap.end(); ++it, i++)
    {
        datasetNames[i] = it->second.get()->getName();
    }
    return datasetNames;
}

std::vector<QString> Core::requestAllDataNames(const std::vector<DataType> dataTypes)
{
    const auto& datasetMap = _dataManager->allSets();

    std::vector<QString> datasetNames(datasetMap.size());

    int i = 0;
    for (auto it = datasetMap.begin(); it != datasetMap.end(); ++it, i++)
    {
        for (DataType dt : dataTypes)
        {
            if (it->second.get()->getDataType() == dt)
            {
                datasetNames[i] = it->second.get()->getName();
            }
        }
    }
    return datasetNames;
}

const void Core::analyzeDataset(const QString kind, const QString& datasetName)
{
    _pluginManager->createAnalysisPlugin(kind, datasetName);
}

const void Core::exportDataset(const QString kind, const QString& datasetName)
{
    _pluginManager->createExporterPlugin(kind, datasetName);
}

QStringList Core::requestPluginKindsByPluginTypeAndDataType(const plugin::Type& pluginType, const DataType& dataType) const
{
    return _pluginManager->requestPluginKindsByPluginTypeAndDataType(pluginType, dataType);
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

void Core::registerEventListener(EventListener* eventListener)
{
    _eventListeners.push_back(eventListener);
}

void Core::unregisterEventListener(EventListener* eventListener)
{
    _eventListeners.erase(std::remove(_eventListeners.begin(), _eventListeners.end(), eventListener), _eventListeners.end());
}

/**
 * Goes through all plug-ins stored in the core and calls the 'dataAdded' function
 * on all plug-ins that inherit from the DataConsumer interface.
 */
void Core::notifyDataAdded(const QString datasetName)
{
    DataType dt = requestData(datasetName).getDataType();

    DataAddedEvent dataEvent;
    dataEvent.dataSetName = datasetName;
    dataEvent.dataType = dt;
    
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataChanged' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataChanged(const QString datasetName)
{
    DataType dt = requestData(datasetName).getDataType();

    DataChangedEvent dataEvent;
    dataEvent.dataSetName = datasetName;
    dataEvent.dataType = dt;

    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

/**
* Goes through all plug-ins stored in the core and calls the 'dataRemoved' function
* on all plug-ins that inherit from the DataConsumer interface.
*/
void Core::notifyDataRemoved(const QString datasetName)
{
    DataType dt = requestData(datasetName).getDataType();

    DataRemovedEvent dataEvent;
    dataEvent.dataSetName = datasetName;
    dataEvent.dataType = dt;

    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

/** Notify all data consumers that a selection has changed. */
void Core::notifySelectionChanged(const QString datasetName)
{
    DataType dt = requestData(datasetName).getDataType();

    SelectionChangedEvent dataEvent;
    dataEvent.dataSetName = datasetName;
    dataEvent.dataType = dt;

    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

/** Notify all event listeners that a dataset has been renamed. */
void Core::notifyDataRenamed(const QString oldName, const QString newName)
{
    DataType dt = requestData(newName).getDataType();

    DataRenamedEvent dataEvent;
    dataEvent.oldName = oldName;
    dataEvent.dataSetName = newName;
    dataEvent.dataType = dt;

    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

hdps::DataHierarchyItem* Core::getDataHierarchyItem(const QString& datasetName)
{
    return _dataHierarchyManager.getHierarchyItem(datasetName);
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
