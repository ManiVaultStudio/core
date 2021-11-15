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
            _dataHierarchyManager->getItem(outputDataset.getId()).addIcon("analysis", analysisPlugin->getIcon());

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

DataSet& Core::addData(const QString& kind, const QString& dataSetGuiName, const DataSet* parentDataset /*= nullptr*/)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager->createPlugin(kind);

    // Request it from the core
    const plugin::RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet    = rawData.createDataSet();
    auto selection  = rawData.createDataSet();

    // Set the properties of the new sets
    fullSet->setGuiName(dataSetGuiName);
    fullSet->setAll(true);

    // Add them to the data manager
    _dataManager->addSet(*fullSet);
    
    _dataManager->addSelection(rawDataName, selection);
    
    // Add the dataset to the hierarchy manager and select the dataset
    _dataHierarchyManager->addItem(*fullSet, const_cast<DataSet*>(parentDataset));
    _dataHierarchyManager->selectItem(*fullSet);
    
    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    // Add data action (available as right-click menu in the data hierarchy widget)
    new DataAction(&_mainWindow, *fullSet);

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

DataSet& Core::createDerivedData(const QString& guiName, const DataSet& sourceDataset, const DataSet* parentDataset /*= nullptr*/)
{
    // Get the data type of the source dataset
    const auto dataType = sourceDataset.getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(dataType._type);

    // Request it from the core
    plugin::RawData& rawData = requestRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    auto derivedDataset = rawData.createDataSet();

    // Mark the full set as derived
    derivedDataset->setSourceDataSetId(sourceDataset.getId());

    // Set properties of the new set
    derivedDataset->setAll(true);
    
    // Add them to the data manager
    _dataManager->addSet(*derivedDataset);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager->addItem(*derivedDataset, parentDataset == nullptr ? const_cast<DataSet*>(&sourceDataset) : const_cast<DataSet*>(parentDataset));

    // Initialize the dataset (e.g. setup default actions for info)
    derivedDataset->init();

    // Add data action (available as right-click menu in the data hierarchy widget)
    new DataAction(&_mainWindow, *derivedDataset);

    return *derivedDataset;
}

DataSet& Core::createSubsetFromSelection(const DataSet& selection, const DataSet& sourceDataset, const QString& guiName, const DataSet* parentDataset /*= nullptr*/, const bool& visible /*= true*/)
{
    // Create a subset with only the indices that were part of the selection set
    auto subset = selection.copy();

    subset->_dataName           = sourceDataset._dataName;
    subset->_sourceDataSetId    = sourceDataset._sourceDataSetId;
    subset->_derived            = sourceDataset._derived;

    // Add the set the core and publish the name of the set to all plug-ins
    _dataManager->addSet(*subset);
    
    // Notify listeners that data was added
    notifyDataAdded(*subset);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager->addItem(*subset, parentDataset == nullptr ? const_cast<DataSet*>(&sourceDataset) : const_cast<DataSet*>(parentDataset), visible);

    // Initialize the dataset (e.g. setup default actions for info)
    subset->init();

    // Add data action (available as right-click menu in the data hierarchy widget)
    new DataAction(&_mainWindow, *subset);

    return *subset;
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

hdps::DataHierarchyItem& Core::getDataHierarchyItem(const QString& dataSetId)
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

void Core::notifyDataAdded(const DataSet& dataset)
{
    // Create data added event
    DataAddedEvent dataEvent(const_cast<DataSet*>(&dataset));

    // Cache the event listeners to prevent crash
    const auto eventListeners = _eventListeners;

    // And notify all listeners
    for (auto listener : eventListeners)
        if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
            listener->onDataEvent(&dataEvent);
}

void Core::notifyDataAboutToBeRemoved(const DataSet& dataset)
{
    // Create data about to be removed event
    DataAboutToBeRemovedEvent dataAboutToBeRemovedEvent(const_cast<DataSet*>(&dataset));

    // Cache the event listeners to prevent timing issues
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

    // Cache the event listeners to prevent timing issues
    const auto eventListeners = _eventListeners;

    // And notify all listeners
    for (auto listener : eventListeners)
        if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
            listener->onDataEvent(&dataRemovedEvent);
}

void Core::notifyDataChanged(const DataSet& dataset)
{
    // Create data changed event
    DataChangedEvent dataEvent(const_cast<DataSet*>(&dataset));

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataEvent);
}

void Core::notifyDataSelectionChanged(const DataSet& dataset)
{
    // Create data selection changed event
    DataSelectionChangedEvent dataSelectionChangedEvent(const_cast<DataSet*>(&dataset));

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataSelectionChangedEvent);
}

void Core::notifyDataGuiNameChanged(const DataSet& dataset, const QString& previousGuiName)
{
    // Create GUI name changed event
    DataGuiNameChangedEvent dataGuiNameChangedEvent(const_cast<DataSet*>(&dataset), previousGuiName);

    // And notify all listeners
    for (EventListener* listener : _eventListeners)
        listener->onDataEvent(&dataGuiNameChangedEvent);
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
