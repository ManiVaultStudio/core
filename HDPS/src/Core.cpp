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
#include "PluginFactory.h"

#include <algorithm>

#define CORE_VERBOSE

using namespace hdps::util;
using namespace hdps::plugin;

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
    setText("HDPS Application");
    setObjectName("HDPS Application");

    _datasetGroupingEnabled = Application::current()->getSetting("Core/DatasetGroupingEnabled", false).toBool();
}

Core::~Core()
{
    // Remove all existing dataset
    removeAllDatasets();

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
            notifyDatasetAdded(outputDataset);

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
    QString rawDataName = _pluginManager->createPlugin(kind)->getName();

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
    //_dataHierarchyManager->selectItems(DataHierarchyItems({ &fullSet->getDataHierarchyItem() }));

    // Initialize the dataset (e.g. setup default actions for info)
    fullSet->init();

    return fullSet;
}

void Core::removeDataset(Dataset<DatasetImpl> dataset)
{
    try
    {
        // Except when the smart pointer to the dataset is invalid
        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

#ifdef CORE_VERBOSE
        qDebug() << "Removing" << dataset->getGuiName() << "from the core";
#endif

        Datasets datasetsToRemove{ dataset };

        // Get children in a top-down manner
        for (auto child : _dataHierarchyManager->getChildren(dataset->getDataHierarchyItem()))
            datasetsToRemove << child->getDataset();

        // Remove datasets bottom-up (this prevents issues with the data hierarchy manager)
        std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

        for (auto datasetToRemove : datasetsToRemove) {

            // Cache dataset GUID and type
            const auto guid = datasetToRemove->getGuid();
            const auto type = datasetToRemove->getDataType();

            notifyDatasetAboutToBeRemoved(datasetToRemove);
            {
                _dataManager->removeDataset(datasetToRemove);
            }
            notifyDatasetRemoved(guid, type);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove dataset", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to remove dataset");
    }
}

void Core::removeAllDatasets()
{
#ifdef CORE_VERBOSE
    qDebug() << "Removing all datasets from the core";
#endif

    for (auto topLevelItem : _dataHierarchyManager->getTopLevelItems())
        removeDataset(topLevelItem->getDataset());

    Application::current()->setCurrentProjectFilePath("");
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
        notifyDatasetAdded(dataset);
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

Dataset<DatasetImpl> Core::createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
{
    // Get the data type of the source dataset
    const auto dataType = sourceDataset->getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->createPlugin(dataType._type)->getName();

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
        notifyDatasetAdded(*subset);

        // Add the dataset to the hierarchy manager
        _dataHierarchyManager->addItem(subset, const_cast<Dataset<DatasetImpl>&>(parentDataset), visible);

        // Initialize the dataset (e.g. setup default actions for info)
        subset->init();

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

const DataManager& Core::getDataManager() const
{
    return *_dataManager.get();
}

hdps::DataHierarchyItem& Core::getDataHierarchyItem(const QString& dataSetId)
{
    return _dataHierarchyManager->getItem(dataSetId);
}

bool Core::isDatasetGroupingEnabled() const
{
    return _datasetGroupingEnabled;
}

hdps::plugin::Plugin* Core::requestPlugin(const QString& kind, const Datasets& datasets /*= Datasets()*/)
{
    try {
        return _pluginManager->createPlugin(kind, datasets);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request plugin from the core", e);
        return nullptr;
    }
    catch (...) {
        exceptionMessageBox("Unable to request plugin from the core");
        return nullptr;
    }
}

QList<QAction*> Core::getPluginActionsByPluginTypeAndDatasets(const plugin::Type& pluginType, const Datasets& datasets) const
{
    return _pluginManager->getPluginActionsByPluginTypeAndDatasets(pluginType, datasets);
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

void Core::notifyDatasetAdded(const Dataset<DatasetImpl>& dataset)
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

void Core::notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset)
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

void Core::notifyDatasetRemoved(const QString& datasetId, const DataType& dataType)
{
    try {

        // Create data removed event
        DataRemovedEvent dataRemovedEvent(nullptr, datasetId, dataType);

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

void Core::notifyDatasetChanged(const Dataset<DatasetImpl>& dataset)
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

void Core::notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset)
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

void Core::notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName)
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

void Core::notifyDatasetLocked(const Dataset<DatasetImpl>& dataset)
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

void Core::notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset)
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

void Core::fromVariantMap(const QVariantMap& variantMap)
{
    _pluginManager->fromVariantMap(variantMap[_pluginManager->getSerializationName()].toMap());
    _dataHierarchyManager->fromVariantMap(variantMap[_dataHierarchyManager->getSerializationName()].toMap());
}

QVariantMap Core::toVariantMap() const
{
    QVariantMap variantMap;

    variantMap[_pluginManager->getSerializationName()]          = _pluginManager->toVariantMap();
    variantMap[_dataHierarchyManager->getSerializationName()]   = _dataHierarchyManager->toVariantMap();

    return variantMap;
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
