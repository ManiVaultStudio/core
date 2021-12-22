#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "event/Event.h"
#include "util/Exception.h"

#include "AnalysisPlugin.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "RawData.h"
#include "Set.h"

#include "DataAction.h"

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
    _datasetGroupingEnabled = Application::current()->getSetting("Core/DatasetGroupingEnabled", false).toBool();
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
            auto outputDataset = analysisPlugin->getOutputDataset();

            // Adjust the data hierarchy icon
            _dataHierarchyManager->getItem(outputDataset->getGuid()).addIcon("analysis", analysisPlugin->getIcon());

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

Dataset<DatasetImpl> Core::addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
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
    _dataManager->addSet(fullSet);

    _dataManager->addSelection(rawDataName, selection);
    
    // Add the dataset to the hierarchy manager and select the dataset
    _dataHierarchyManager->addItem(fullSet, const_cast<Dataset<DatasetImpl>&>(parentDataset));
    _dataHierarchyManager->selectItem(fullSet);
    
    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    // Add data action (available as right-click menu in the data hierarchy widget)
    new DataAction(&_mainWindow, *fullSet);

    return fullSet;
}

Dataset<DatasetImpl> Core::copyDataset(const Dataset<DatasetImpl>& dataset, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
{
    try
    {
        // Except if the source dataset is invalid
        if (!dataset.isValid())
            throw std::runtime_error("Source dataset is invalid");

        // Copy the dataset
        auto datasetCopy = dataset->copy();

        // Adjust GUI name
        datasetCopy->setGuiName(dataSetGuiName);

        // Establish parent
        auto parent = parentDataset.isValid() ? const_cast<Dataset<DatasetImpl>&>(parentDataset) : (dataset->getDataHierarchyItem().hasParent() ? dataset->getParent() : Dataset<DatasetImpl>());

        // Add to the data hierarchy manager
        _dataHierarchyManager->addItem(const_cast<Dataset<DatasetImpl>&>(datasetCopy), parent);

        // Notify others that data was added
        notifyDataAdded(dataset);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to copy dataset", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to copy dataset");
    }

    return Dataset<DatasetImpl>();
}

void Core::removeDatasets(const QVector<Dataset<DatasetImpl>> datasets, const bool& recursively /*= false*/)
{
    // Remove datasets one by one
    for (const auto& dataset : datasets) {

        // Cache the data type because later on the dataset has already been removed
        const auto dataType = dataset->getDataType();

        // Notify listeners that the dataset is about to be removed
        notifyDataAboutToBeRemoved(dataset);
        
        // Remove the dataset from the data manager
        _dataManager->removeDataset(dataset);

        // Remove the dataset from the data hierarchy manager
        _dataHierarchyManager->removeItem(datasets.first(), true);

        // Notify listeners that the dataset is removed
        notifyDataRemoved(dataset->getGuid(), dataType);
    }
}

Dataset<DatasetImpl> Core::createDerivedData(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
{
    // Get the data type of the source dataset
    const auto dataType = sourceDataset->getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(dataType._type);

    // Request it from the core
    plugin::RawData& rawData = requestRawData(pluginName);

    // Create an initial full set, but no selection because it is shared with the source data
    auto derivedDataset = rawData.createDataSet();

    // Mark the full set as derived and set the GUI name
    derivedDataset->setSourceDataSet(sourceDataset);
    derivedDataset->setGuiName(guiName);

    // Set properties of the new set
    derivedDataset->setAll(true);
    
    // Add them to the data manager
    _dataManager->addSet(*derivedDataset);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager->addItem(derivedDataset, !parentDataset.isValid() ? const_cast<Dataset<DatasetImpl>&>(sourceDataset) : const_cast<Dataset<DatasetImpl>&>(parentDataset));

    // Initialize the dataset (e.g. setup default actions for info)
    derivedDataset->init();

    // Add data action (available as right-click menu in the data hierarchy widget)
    new DataAction(&_mainWindow, *derivedDataset);

    return Dataset<DatasetImpl>(*derivedDataset);
}

Dataset<DatasetImpl> Core::createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible /*= true*/)
{
    try
    {
        // Create a subset with only the indices that were part of the selection set
        auto subset = selection->copy();

        // Assign source dataset to subset
        *subset = *const_cast<Dataset<DatasetImpl>&>(sourceDataset);

        subset->setAll(false);
        subset->setGuiName(guiName);

        // Add the set the core and publish the name of the set to all plug-ins
        _dataManager->addSet(*subset);
    
        // Notify listeners that data was added
        notifyDataAdded(*subset);

        // Add the dataset to the hierarchy manager
        _dataHierarchyManager->addItem(subset, const_cast<Dataset<DatasetImpl>&>(parentDataset), visible);

        // Initialize the dataset (e.g. setup default actions for info)
        subset->init();

        // Add data action (available as right-click menu in the data hierarchy widget)
        new DataAction(&_mainWindow, *subset);

        return subset;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create subset from selection", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to create subset from selection");
    }

    return Dataset<DatasetImpl>();
}

plugin::RawData& Core::requestRawData(const QString& name)
{
    try
    {
        return _dataManager->getRawData(name);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request raw data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request raw data");
    }
}

Dataset<DatasetImpl> Core::requestDataset(const QString& dataSetId)
{
    try
    {
        return Dataset<DatasetImpl>(_dataManager->getSet(dataSetId));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request dataset by identifier", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request dataset by identifier");
    }

    return Dataset<DatasetImpl>();
}

QVector<Dataset<DatasetImpl>> Core::requestAllDataSets(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/)
{
    // Resulting smart pointers to datasets
    QVector<Dataset<DatasetImpl>> allDataSets;

    try
    {
        // Get a map of all datasets
        const auto& dataSetsMap = _dataManager->allSets();

        // And reserve the proper amount of dataset references
        allDataSets.reserve(static_cast<std::int32_t>(dataSetsMap.size()));

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

const void Core::analyzeDataset(const QString& kind, Dataset<DatasetImpl>& dataSet)
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

const void Core::exportDataset(const QString& kind, Dataset<DatasetImpl>& dataSet)
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

const void Core::viewDataset(const QString& kind, const Datasets& datasets)
{
    try {
        _pluginManager->createViewPlugin(kind, datasets);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to view dataset", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to view dataset");
    }
}

hdps::DataHierarchyItem& Core::getDataHierarchyItem(const QString& dataSetId)
{
    return _dataHierarchyManager->getItem(dataSetId);
}

bool Core::isDatasetGroupingEnabled() const
{
    return _datasetGroupingEnabled;
}

QStringList Core::getPluginKindsByPluginTypeAndDataTypes(const plugin::Type& pluginType, const QVector<DataType>& dataTypes /*= QVector<DataType>()*/) const
{
    return _pluginManager->getPluginKindsByPluginTypeAndDataTypes(pluginType, dataTypes);
}

Dataset<DatasetImpl> Core::requestSelection(const QString& name)
{
    return Dataset<DatasetImpl>(_dataManager->getSelection(name));
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

void Core::notifyDataAdded(const Dataset<DatasetImpl>& dataset)
{
    try {

        // Create data added event
        DataAddedEvent dataEvent(dataset);

        // Cache the event listeners to prevent crash
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data was added", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data was added");
    }
}

void Core::notifyDataAboutToBeRemoved(const Dataset<DatasetImpl>& dataset)
{
    try {

        // Create data about to be removed event
        DataAboutToBeRemovedEvent dataAboutToBeRemovedEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataAboutToBeRemovedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is about to be removed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is about to be removed");
    }
}

void Core::notifyDataRemoved(const QString& datasetId, const DataType& dataType)
{
    try {

        // Create data removed event
        DataRemovedEvent dataRemovedEvent(nullptr, datasetId);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataRemovedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is removed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is removed");
    }
}

void Core::notifyDataChanged(const Dataset<DatasetImpl>& dataset)
{
    try {
    
        // Create data changed event
        DataChangedEvent dataEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is changed");
    }
}

void Core::notifyDataSelectionChanged(const Dataset<DatasetImpl>& dataset)
{
    try {

        // Create data selection changed event
        DataSelectionChangedEvent dataSelectionChangedEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataSelectionChangedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data selection has changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data selection has changed");
    }
}

void Core::notifyDataGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName)
{
    try {

        // Create GUI name changed event
        DataGuiNameChangedEvent dataGuiNameChangedEvent(dataset, previousGuiName);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataGuiNameChangedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data GUI name has changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data GUI name has changed");
    }
}

void Core::notifyDataChildAdded(const Dataset<DatasetImpl>& parentDataset, const Dataset<DatasetImpl>& childDataset)
{
    try {

        // Except if parent dataset is not valid
        if (!parentDataset.isValid())
            throw std::runtime_error("Parent dataset is invalid");

        // Except if child dataset is not valid
        if (!childDataset.isValid())
            throw std::runtime_error("Child dataset is invalid");

        // Create data child added event
        DataChildAddedEvent dataChildAddedEvent(parentDataset, childDataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataChildAddedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data child was added", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data child was added");
    }
}

void Core::notifyDataChildRemoved(const Dataset<DatasetImpl>& parentDataset, const QString& childDatasetGuid)
{
    try {

        // Except if parent dataset is not valid
        if (!parentDataset.isValid())
            throw std::runtime_error("Parent dataset is invalid");

        // Create data child removed event
        DataChildRemovedEvent dataChildRemovedEvent(parentDataset, childDatasetGuid);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataChildRemovedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data child was removed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data child was removed");
    }
}

void Core::notifyDataLocked(const Dataset<DatasetImpl>& dataset)
{
    try {

        // Except if dataset is not valid
        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        // Create data locked event
        DataLockedEvent dataLockedEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataLockedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data was locked", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data was locked");
    }
}

void Core::notifyDataUnlocked(const Dataset<DatasetImpl>& dataset)
{
    try {

        // Except if dataset is not valid
        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        // Create data unlocked event
        DataUnlockedEvent dataUnlockedEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataUnlockedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data was unlocked", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data was unlocked");
    }
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

void Core::setDatasetGroupingEnabled(const bool& datasetGroupingEnabled)
{
    if (datasetGroupingEnabled == _datasetGroupingEnabled)
        return;

    _datasetGroupingEnabled = datasetGroupingEnabled;

    // Save the setting
    Application::current()->setSetting("Core/DatasetGroupingEnabled", _datasetGroupingEnabled);
}

} // namespace hdps
