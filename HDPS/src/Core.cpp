#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "exceptions/SetNotFoundException.h"
#include "util/DatasetRef.h"
#include "util/Exception.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "RawData.h"
#include "Set.h"

#include "DataAction.h"

#include <QMessageBox>
#include <algorithm>

using namespace hdps::util;

namespace hdps
{

Core::Core(gui::MainWindow& mainWindow) :
    _mainWindow(mainWindow),
    _pluginManager(),
    _dataManager(),
    _dataHierarchyManager(),
    _plugins(),
    _eventListeners()
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
    _pluginManager          = std::make_unique<plugin::PluginManager>(*this);
    _dataManager            = std::make_unique<DataManager>(this);
    _dataHierarchyManager   = std::make_unique<DataHierarchyManager>(&_mainWindow);

    _pluginManager->loadPlugins();
}

hdps::DataHierarchyManager& Core::getDataHierarchyManager()
{
    return *_dataHierarchyManager;
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

            // Get reference to the analysis output dataset
            auto& outputDataset = analysisPlugin->getOutputDataset();

            // Adjust the data hierarchy icon
            _dataHierarchyManager->getItem(outputDataset.getId())->addIcon("analysis", analysisPlugin->getIcon());

            // Notify listeners that a dataset was added
            notifyDataAdded(outputDataset);

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

DataSet& Core::addData(const QString& kind, const QString& dataSetGuiName, const DataSet* parentDataSet /*= nullptr*/)
{
    

    
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);

    // Request it from the core
    const plugin::RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet    = rawData.createDataSet();
    auto selection  = rawData.createDataSet();
    /*
    // Set the properties of the new sets
    fullSet->setAll(true);

    // Add them to the data manager
    QString setName = _dataManager->addSet(nameRequest, fullSet);
    
    _dataManager->addSelection(rawDataName, selection);
    
    // Add the dataset to the hierarchy manager and select the dataset
    _dataHierarchyManager->addItem(setName, parentDatasetName);
    _dataHierarchyManager->selectItem(setName);
    
    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    new DataAction(&_mainWindow, setName);

    return setName;
    */

    return *fullSet;
}

void Core::removeDatasets(const QVector<DataSet*> datasets, const bool& recursively /*= false*/)
{
    // Remove datasets one by one
    for (const auto& dataset : datasets) {

        // Cache the data type because later on the dataset has already been removed
        const auto dataType = dataset->getDataType();

        // Notify listeners that the dataset is about to be removed
        notifyDataAboutToBeRemoved(*dataset);
        
        // Remove the dataset from the data manager
        //_dataManager->removeDataset(*dataset);

        // Notify listeners that the dataset is removed
        notifyDataRemoved(dataset->getId(), dataType);
    }
}

const QString Core::createDerivedData(const QString& nameRequest, const QString& sourceDatasetName, const QString& dataHierarchyParent /*= ""*/)
{
    /*
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
    _dataManager->addSet(*fullSet);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager->addItem(fullSet->getId(),, dataHierarchyParent.isEmpty() ? sourceDatasetName : dataHierarchyParent);

    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    new DataAction(&_mainWindow, setName);

    return setName;
    */

    return "";
}

DataSet& Core::createSubsetFromSelection(const DataSet& selection, const DataSet& sourceDataset, const QString& guiName, const DataSet* parentDataset /*= nullptr*/, const bool& visible /*= true*/)
{
    // Create a new set with only the indices that were part of the selection set
    auto newSet = selection.copy();

    newSet->_dataName       = sourceDataset._dataName;
    newSet->_sourceSetName  = sourceDataset._sourceSetName;
    newSet->_derived        = sourceDataset._derived;

    /*
    // Add the set the core and publish the name of the set to all plug-ins
    const auto setName = _dataManager->addSet(newSetName, newSet);
    
    // Notify listeners that data was added
    notifyDataAdded(*newSet);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager->addItem(newSet->getId(), dataHierarchyParent.isEmpty() ? sourceSet.getName() : dataHierarchyParent, visible);

    // Initialize the dataset (e.g. setup default actions for info)
    newSet->init();

    new DataAction(&_mainWindow, setName);
    */

    return *newSet;
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

DataSet& Core::requestData(const QString& dataSetId)
{
    try
    {
        return _dataManager->getSet(dataSetId);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request dataset by identifier", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request dataset by identifier");
    }
}

QVector<DataSet*> Core::requestAllDataSets(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/)
{
    // Result
    QVector<DataSet*> allDataSets;

    try
    {
        // Get a map of all datasets
        const auto& dataSetsMap = _dataManager->allSets();

        // And reserve the proper amount of dataset references
        allDataSets.reserve(dataSetsMap.size());

        // Add dataset references one by one
        for (auto it = dataSetsMap.begin(); it != dataSetsMap.end(); it++) {
            if (dataTypes.isEmpty()) {
                allDataSets << it->second.get();
            }
            else {
                for (const auto& dataType : dataTypes)
                    if (it->second.get()->getDataType() == dataType)
                        allDataSets << it->second.get();
            }
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request all datasets", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request all datasets");
    }

    return allDataSets;
}

hdps::plugin::Plugin& Core::requestAnalysis(const QString& kind)
{
    try {
        auto analysisPluginsMap = std::map<QString, hdps::plugin::Plugin*>();

        for (const auto& analysisPlugin : _plugins[plugin::Type::ANALYSIS])
            analysisPluginsMap[analysisPlugin->getName()] = analysisPlugin.get();

        return *analysisPluginsMap[kind];
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request analysis", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request analysis");
    }
}

const void Core::analyzeDataset(const QString& kind, DataSet& dataSet)
{
    try {
        _pluginManager->createAnalysisPlugin(kind, dataSet);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create analysis plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create analysis plugin");
    }
}

const void Core::importDataset(const QString& kind)
{
    try {
        _pluginManager->createPlugin(kind);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create import plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create import plugin");
    }
}

const void Core::exportDataset(const QString& kind, DataSet& dataSet)
{
    try {
        _pluginManager->createExporterPlugin(kind, dataSet);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create export plugin", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create export plugin");
    }
}

hdps::DataHierarchyItem* Core::getDataHierarchyItem(const QString& dataSetId)
{
    return _dataHierarchyManager->getItem(dataSetId);
}

QStringList Core::getPluginKindsByPluginTypeAndDataTypes(const plugin::Type& pluginType, const QVector<DataType>& dataTypes /*= QVector<DataType>()*/) const
{
    return _pluginManager->getPluginKindsByPluginTypeAndDataTypes(pluginType, dataTypes);
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

QString Core::getPluginGuiName(const QString& pluginKind) const
{
    return _pluginManager->getPluginGuiName(pluginKind);
}

QIcon Core::getPluginIcon(const QString& pluginKind) const
{
    return _pluginManager->getPluginIcon(pluginKind);
}

void Core::notifyDataAdded(DataSet& dataset)
{
    // Create data added event
    DataAddedEvent dataEvent(&dataset);

    // Cache the event listeners to prevent crash
    const auto eventListeners = _eventListeners;

    // And notify all listeners
    for (auto listener : eventListeners)
        if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
            listener->onDataEvent(&dataEvent);
}

void Core::notifyDataAboutToBeRemoved(DataSet& dataset)
{
    // Create data about to be removed event
    DataAboutToBeRemovedEvent dataAboutToBeRemovedEvent(&dataset);

    // Cache the event listeners to prevent crash
    const auto eventListeners = _eventListeners;

    // And notify all listeners
    for (auto listener : eventListeners)
        if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
            listener->onDataEvent(&dataAboutToBeRemovedEvent);
}

void Core::notifyDataRemoved(const QString& datasetId, const DataType& dataType)
{
    // Create data removed event
    DataRemovedEvent dataRemovedEvent(nullptr, datasetId);

    // Cache the event listeners to prevent crash
    const auto eventListeners = _eventListeners;

    // And notify all listeners
    for (auto listener : eventListeners)
        if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
            listener->onDataEvent(&dataRemovedEvent);
}

void Core::notifyDataChanged(DataSet& dataset)
{
    // Create data changed event
    DataChangedEvent dataEvent(&dataset);

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

void Core::notifySelectionChanged(DataSet& dataset)
{
    // Create selection changed event
    SelectionChangedEvent selectionChangedEvent(&dataset);

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&selectionChangedEvent);
}

void Core::notifyGuiNameChanged(DataSet& dataset, const QString& previousGuiName)
{
    // Create GUI name changed event
    GuiNameChangedEvent guiNameChangedEvent(&dataset, previousGuiName);

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&guiNameChangedEvent);
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
