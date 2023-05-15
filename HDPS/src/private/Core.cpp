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
    _actionsManager(),
    _pluginManager(),
    _eventManager(),
    _dataManager(),
    _dataHierarchyManager(),
    _workspaceManager(),
    _projectManager(),
    _settingsManager()
{
    _datasetGroupingEnabled = Application::current()->getSetting("Core/DatasetGroupingEnabled", false).toBool();
}

Core::~Core()
{
    _pluginManager.reset();
}

void Core::init()
{
    CoreInterface::init();

    _actionsManager.reset(new ActionsManager());
    _pluginManager.reset(new PluginManager());
    _eventManager.reset(new EventManager());
    _dataManager.reset(new DataManager());
    _dataHierarchyManager.reset(new DataHierarchyManager());
    _workspaceManager.reset(new WorkspaceManager());
    _projectManager.reset(new ProjectManager());
    _settingsManager.reset(new SettingsManager());

    _actionsManager->initialize();
    _pluginManager->initialize();
    _eventManager->initialize();
    _dataManager->initialize();
    _dataHierarchyManager->initialize();
    _workspaceManager->initialize();
    _projectManager->initialize();
    _settingsManager->initialize();
}

void Core::reset()
{
    _actionsManager->reset();
    _pluginManager->reset();
    _eventManager->reset();
    _dataManager->reset();
    _dataHierarchyManager->reset();
    _workspaceManager->reset();
    _projectManager->reset();
    _settingsManager->reset();
}

void Core::addPlugin(plugin::Plugin* plugin)
{
    switch (plugin->getType())
    {
        // If the plugin is RawData, then add it to the data manager
        case plugin::Type::DATA:
        {
            _dataManager->addRawData(dynamic_cast<plugin::RawData*>(plugin));
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
                events().notifyDatasetAdded(analysisPlugin->getOutputDataset());

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
    QString rawDataName = _pluginManager->requestPlugin(kind)->getName();

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

            events().notifyDatasetAboutToBeRemoved(datasetToRemove);
            {
                _dataManager->removeDataset(datasetToRemove);
            }
            events().notifyDatasetRemoved(guid, type);
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

Dataset<DatasetImpl> Core::createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset /*= Dataset<DatasetImpl>()*/)
{
    // Get the data type of the source dataset
    const auto dataType = sourceDataset->getDataType();

    // Create a new plugin of the given kind
    QString pluginName = _pluginManager->requestPlugin(dataType._type)->getName();

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

        // Set a pointer to the original full dataset, if the source is another subset, we take their pointer
        subset->_fullDataset = sourceDataset->isFull() ? sourceDataset : sourceDataset->_fullDataset;

        // Add the set the core and publish the name of the set to all plug-ins
        _dataManager->addSet(*subset);

        // Add the dataset to the hierarchy manager
        _dataHierarchyManager->addItem(subset, const_cast<Dataset<DatasetImpl>&>(parentDataset), visible);

        // Notify listeners that data was added
        events().notifyDatasetAdded(*subset);

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
    plugin::RawData* data = nullptr;

    try
    {
        data = &(_dataManager->getRawData(name));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to request raw data", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to request raw data");
    }

    return *data;
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
    QVector<Dataset<DatasetImpl>> allDataSets;

    try
    {
        const auto& datasets = _dataManager->allSets();

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
    return *_dataManager;
}

AbstractPluginManager& Core::getPluginManager()
{
    return *_pluginManager;
}

AbstractActionsManager& Core::getActionsManager()
{
    return *_actionsManager;
}

AbstractProjectManager& Core::getProjectManager()
{
    return *_projectManager;
}

AbstractSettingsManager& Core::getSettingsManager()
{
    return *_settingsManager;
}

AbstractDataHierarchyManager& Core::getDataHierarchyManager()
{
    return *_dataHierarchyManager;
}

AbstractEventManager& Core::getEventManager()
{
    return *_eventManager;
}

AbstractWorkspaceManager& Core::getWorkspaceManager()
{
    return *_workspaceManager;
}

DataHierarchyItem& Core::getDataHierarchyItem(const QString& dataSetId)
{
    return _dataHierarchyManager->getItem(dataSetId);
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

Dataset<DatasetImpl> Core::requestSelection(const QString& name)
{
    return Dataset<DatasetImpl>(_dataManager->getSelection(name));
}

void Core::setDatasetGroupingEnabled(const bool& datasetGroupingEnabled)
{
    if (datasetGroupingEnabled == _datasetGroupingEnabled)
        return;

    _datasetGroupingEnabled = datasetGroupingEnabled;

    // Save the setting
    Application::current()->setSetting("Core/DatasetGroupingEnabled", _datasetGroupingEnabled);
}

}
