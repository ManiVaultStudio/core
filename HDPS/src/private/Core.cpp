#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"
#include "GroupDataDialog.h"

#include <LoaderPlugin.h>
#include <WriterPlugin.h>
#include <ViewPlugin.h>
#include <AnalysisPlugin.h>
#include <RawData.h>
#include <Set.h>
#include <PluginFactory.h>
#include <ViewPlugin.h>
#include <event/Event.h>
#include <util/Exception.h>

#include <algorithm>

//#define CORE_VERBOSE

using namespace hdps;
using namespace hdps::util;
using namespace hdps::plugin;
using namespace hdps::gui;

namespace hdps {

Core::Core() :
    _pluginManager(),
    _dataManager(),
    _dataHierarchyManager(),
    _layoutManager(),
    _actionsManager(),
    _eventListeners()
{
    _datasetGroupingEnabled = Application::current()->getSetting("Core/DatasetGroupingEnabled", false).toBool();
}

Core::~Core()
{
    // Remove all existing dataset
    removeAllDatasets();

    // Delete the plugin manager
    _pluginManager.reset();
}

void Core::init()
{
    _pluginManager.loadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    switch (plugin->getType())
    {
        // If the plugin is RawData, then add it to the data manager
        case plugin::Type::DATA:
        {
            _dataManager.addRawData(dynamic_cast<plugin::RawData*>(plugin));
            break;
        }

        case plugin::Type::VIEW:
            break;
    }

    // Initialize the plugin after it has been added to the core
    plugin->init();

    switch (plugin->getType())
    {
        case plugin::Type::ANALYSIS:
        {
            auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

            if (analysisPlugin)
                notifyDatasetAdded(analysisPlugin->getOutputDataset());

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

Dataset<DatasetImpl> Core::addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/, const QString& guid /*= ""*/)
{
    // Create a new plugin of the given kind
    QString rawDataName = _pluginManager.createPlugin(kind)->getName();

    // Request it from the core
    const plugin::RawData& rawData = requestRawData(rawDataName);

    // Create an initial full set and an empty selection belonging to the raw data
    auto fullSet    = rawData.createDataSet(guid);
    auto selection  = rawData.createDataSet();

    // Set the properties of the new sets
    fullSet->setGuiName(dataSetGuiName);
    fullSet->setAll(true);

    // Set pointer of full dataset to itself just to avoid having to be wary of this not being set
    fullSet->_fullDataset = fullSet;

    // Add them to the data manager
    _dataManager.addSet(fullSet);

    _dataManager.addSelection(rawDataName, selection);

    // Add the dataset to the hierarchy manager and select the dataset
    _dataHierarchyManager.addItem(fullSet, const_cast<Dataset<DatasetImpl>&>(parentDataset));
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
        for (auto child : _dataHierarchyManager.getChildren(dataset->getDataHierarchyItem()))
            datasetsToRemove << child->getDataset();

        // Remove datasets bottom-up (this prevents issues with the data hierarchy manager)
        std::reverse(datasetsToRemove.begin(), datasetsToRemove.end());

        for (auto datasetToRemove : datasetsToRemove) {

            // Cache dataset GUID and type
            const auto guid = datasetToRemove->getGuid();
            const auto type = datasetToRemove->getDataType();

            notifyDatasetAboutToBeRemoved(datasetToRemove);
            {
                _dataManager.removeDataset(datasetToRemove);
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

    for (auto topLevelItem : _dataHierarchyManager.getTopLevelItems())
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
        _dataHierarchyManager.addItem(const_cast<Dataset<DatasetImpl>&>(datasetCopy), parent);

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
    QString pluginName = _pluginManager.createPlugin(dataType._type)->getName();

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
    _dataManager.addSet(*derivedDataset);

    // Add the dataset to the hierarchy manager
    _dataHierarchyManager.addItem(derivedDataset, !parentDataset.isValid() ? const_cast<Dataset<DatasetImpl>&>(sourceDataset) : const_cast<Dataset<DatasetImpl>&>(parentDataset));

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

        // Set a pointer to the original full dataset, if the source is another subset, we take their pointer
        subset->_fullDataset = sourceDataset->isFull() ? sourceDataset : sourceDataset->_fullDataset;

        // Add the set the core and publish the name of the set to all plug-ins
        _dataManager.addSet(*subset);

        // Add the dataset to the hierarchy manager
        _dataHierarchyManager.addItem(subset, const_cast<Dataset<DatasetImpl>&>(parentDataset), visible);

        // Notify listeners that data was added
        notifyDatasetAdded(*subset);

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
        return _dataManager.getRawData(name);
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
        return Dataset<DatasetImpl>(_dataManager.getSet(dataSetId));
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
    QVector<Dataset<DatasetImpl>> allDataSets;

    try
    {
        const auto& datasets = _dataManager.allSets();

        for (const auto dataset : datasets) {
            if (dataTypes.isEmpty()) {
                allDataSets << dataset;
            }
            else {
                for (const auto& dataType : dataTypes)
                    if (dataset->getDataType() == dataType)
                        allDataSets << dataset;
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

AbstractDataManager& Core::getDataManager()
{
    return _dataManager;
}

AbstractPluginManager& Core::getPluginManager()
{
    return _pluginManager;
}

AbstractActionsManager& Core::getActionsManager()
{
    return _actionsManager;
}

AbstractDataHierarchyManager& Core::getDataHierarchyManager()
{
    return _dataHierarchyManager;
}

AbstractLayoutManager& Core::getLayoutManager()
{
    return _layoutManager;
}

DataHierarchyItem& Core::getDataHierarchyItem(const QString& dataSetId)
{
    return _dataHierarchyManager.getItem(dataSetId);
}

bool Core::isDatasetGroupingEnabled() const
{
    return _datasetGroupingEnabled;
}

Dataset<DatasetImpl> Core::groupDatasets(const Datasets& datasets, const QString& guiName /*= ""*/)
{
    try {
        const auto createGroupDataset = [this, &datasets](const QString& guiName) -> Dataset<DatasetImpl> {
            auto groupDataset = addDataset(datasets.first()->getRawDataKind(), guiName);

            groupDataset->setProxyMembers(datasets);

            return groupDataset;
        };

        if (guiName.isEmpty()) {
            if (Application::current()->getSetting("AskForGroupName", true).toBool()) {
                GroupDataDialog groupDataDialog(nullptr, datasets);

                if (groupDataDialog.exec() == 1)
                    return createGroupDataset(groupDataDialog.getGroupName());
                else
                    return Dataset<DatasetImpl>();
            }
            else {
                QStringList datasetNames;

                for (const auto& dataset : datasets)
                    datasetNames << dataset->getGuiName();

                return createGroupDataset(datasetNames.join("+"));
            }
        }
        else {
            return createGroupDataset(guiName);
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to group data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to group data");
    }
        
    return Dataset<DatasetImpl>();
}

hdps::plugin::Plugin* Core::requestPlugin(const QString& kind, Datasets datasets /*= Datasets()*/)
{
    try {
        return _pluginManager.createPlugin(kind, datasets);

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

hdps::plugin::ViewPlugin* Core::requestViewPlugin(const QString& kind, plugin::ViewPlugin* dockToViewPlugin /*= nullptr*/, gui::DockAreaFlag dockArea /*= gui::DockAreaFlag::Right*/, Datasets datasets /*= Datasets()*/)
{
    auto viewPlugin = dynamic_cast<hdps::plugin::ViewPlugin*>(_pluginManager.createPlugin(kind, datasets));

    if (viewPlugin)
        Application::core()->getLayoutManager().addViewPlugin(viewPlugin, dockToViewPlugin, dockArea);

    return viewPlugin;
}

Dataset<DatasetImpl> Core::requestSelection(const QString& name)
{
    return Dataset<DatasetImpl>(_dataManager.getSelection(name));
}

void Core::registerEventListener(EventListener* eventListener)
{
    _eventListeners.push_back(eventListener);
}

void Core::unregisterEventListener(EventListener* eventListener)
{
    _eventListeners.erase(std::remove(_eventListeners.begin(), _eventListeners.end(), eventListener), _eventListeners.end());
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

void Core::notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets /*= nullptr*/)
{
    try {

        // Do not notify if the dataset has already been notified
        if (ignoreDatasets != nullptr && ignoreDatasets->contains(dataset))
            return;

        if (ignoreDatasets != nullptr)
            *ignoreDatasets << dataset;

        QStringList datasetNotifiedString;

#ifdef CORE_VERBOSE
        if (ignoreDatasets != nullptr)
            for (auto datasetNotified : *ignoreDatasets)
                datasetNotifiedString << datasetNotified->getGuiName();

        qDebug() << __FUNCTION__ << dataset->getGuiName() << datasetNotifiedString;
#endif

        //qDebug() << __FUNCTION__ << dataset->getGuiName();

        Datasets notified{ dataset };

        if (ignoreDatasets == nullptr)
            ignoreDatasets = &notified;

        const auto callNotifyDatasetSelectionChanged = [this, dataset, ignoreDatasets](Dataset<DatasetImpl> notifyDataset) -> void {
            if (ignoreDatasets != nullptr && ignoreDatasets->contains(notifyDataset))
                return;

            notifyDatasetSelectionChanged(notifyDataset, ignoreDatasets);
        };

        DataSelectionChangedEvent dataSelectionChangedEvent(dataset);

        // Cache the event listeners to prevent timing issues
        const auto eventListeners = _eventListeners;

        // And notify all listeners
        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                listener->onDataEvent(&dataSelectionChangedEvent);

        // Notify full dataset if dataset is not full
        if (!dataset->isFull())
            callNotifyDatasetSelectionChanged(dataset->getFullDataset<DatasetImpl>());

        // Notify all proxy members if the dataset is a proxy
        if (dataset->isProxy())
            for (auto proxyMember : dataset->getProxyMembers())
                callNotifyDatasetSelectionChanged(proxyMember->getSourceDataset<DatasetImpl>());

        // Iterate over all datasets, establish if they somehow have a relation with the supplied dataset and possibly notify the candidate dataset
        for (auto candidateDataset : requestAllDataSets()) {

            // Continue to the next candidate if it should be ignored
            if (ignoreDatasets != nullptr && ignoreDatasets->contains(candidateDataset))
                continue;

            // Do not notify the supplied dataset
            if (candidateDataset == dataset)
                continue;

            if (candidateDataset->isDerivedData() && candidateDataset->getSourceDataset<DatasetImpl>()->getRawDataName() == dataset->getSourceDataset<DatasetImpl>()->getRawDataName())
                callNotifyDatasetSelectionChanged(candidateDataset);
                
            if (candidateDataset->getRawDataName() == dataset->getRawDataName())
                callNotifyDatasetSelectionChanged(candidateDataset);

            if (candidateDataset->isProxy() && candidateDataset->getProxyMembers().contains(dataset))
                callNotifyDatasetSelectionChanged(candidateDataset);
        }

        // Notify linked data
        for (const LinkedData& ld : dataset->getLinkedData())
            callNotifyDatasetSelectionChanged(ld.getTargetDataset());
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

void Core::setDatasetGroupingEnabled(const bool& datasetGroupingEnabled)
{
    if (datasetGroupingEnabled == _datasetGroupingEnabled)
        return;

    _datasetGroupingEnabled = datasetGroupingEnabled;

    // Save the setting
    Application::current()->setSetting("Core/DatasetGroupingEnabled", _datasetGroupingEnabled);
}

void Core::fromVariantMap(const QVariantMap& variantMap)
{
    _pluginManager.fromVariantMap(variantMap[_pluginManager.getSerializationName()].toMap());
    _dataHierarchyManager.fromVariantMap(variantMap[_dataHierarchyManager.getSerializationName()].toMap());
    _layoutManager.fromVariantMap(variantMap[_layoutManager.getSerializationName()].toMap());
    _actionsManager.fromVariantMap(variantMap[_actionsManager.getSerializationName()].toMap());
}

QVariantMap Core::toVariantMap() const
{
    QVariantMap variantMap;

    variantMap[_pluginManager.getSerializationName()]           = _pluginManager.toVariantMap();
    variantMap[_dataHierarchyManager.getSerializationName()]    = _dataHierarchyManager.toVariantMap();
    variantMap[_layoutManager.getSerializationName()]           = _layoutManager.toVariantMap();
    variantMap[_actionsManager.getSerializationName()]          = _actionsManager.toVariantMap();

    return variantMap;
}
}
