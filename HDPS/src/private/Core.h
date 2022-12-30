#pragma once

#include <CoreInterface.h>
#include <PluginType.h>
#include <DataType.h>

#include <event/EventListener.h>

#include "DataManager.h"
#include "PluginManager.h"
#include "ActionsManager.h"
#include "DataHierarchyManager.h"
#include "WorkspaceManager.h"
#include "ProjectManager.h"
#include "SettingsManager.h"

#include <memory>
#include <unordered_map>
#include <vector>

using namespace hdps;

namespace hdps {

class Core : public CoreInterface
{
public:

    /** Default constructor */
    Core();

    /** Default destructor */
    ~Core();

public:

    /** Resets the entire core implementation */
    void reset() override;

public: // Data access

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to the added dataset
     */
    Dataset<DatasetImpl> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& guid = "") override;

    /**
     * Removes a single dataset
     * @param dataset Smart pointer to the dataset to remove
     */
    void removeDataset(Dataset<DatasetImpl> dataset) override final;

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to root in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    Dataset<DatasetImpl> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) override;

    /**
     * Creates a copy of the given selection set, adds the new set to the data manager and notifies all data consumers of the new set
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (sourceSetName if not valid)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true) override;

    /**
     * Requests a dataset from the core by dataset GUID (if no such instance can be found a fatal error is thrown)
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    Dataset<DatasetImpl> requestDataset(const QString& datasetGuid) override;

    /**
     * Returns all data sets that are present in the core, filtered by data type(s)
     * Returns all data sets in case of an empty filter
     * @param dataTypes Data types to filter
     * @return Vector of references to datasets
     */
    QVector<Dataset<DatasetImpl>> requestAllDataSets(const QVector<DataType>& dataTypes = QVector<DataType>()) override;

protected: // Data access

    /**
     * Requests an instance of a data type plugin from the core which has the same unique name as the given parameter, if no such instance can be found a fatal error is thrown
     * @param datasetName Name of the dataset
     */
    plugin::RawData& requestRawData(const QString& name) override;

    /**
     * Request a selection from the data manager by its corresponding raw data name.
     * @param rawdataName Name of the raw data
     */
    Dataset<DatasetImpl> requestSelection(const QString& rawDataName) override;

public: // Data grouping

    /**
     * Groups \p datasets into one dataset
     * @param datasets Two or more datasets to group  (if empty, the user will be prompted for a name)
     * @return Smart pointer to created group dataset
     */
    Dataset<DatasetImpl> groupDatasets(const Datasets& datasets, const QString& guiName = "");

public: // Data hierarchy

    /**
     * Get data hierarchy item by dataset GUI
     * @param datasetGuid Globally unique identifier of the dataset
     * @return Reference to data hierarchy item
     */
    DataHierarchyItem& getDataHierarchyItem(const QString& datasetGuid) override;

public: // Dataset grouping

    /** Get whether dataset grouping is enabled or not */
    bool isDatasetGroupingEnabled() const override;

    /** Get whether dataset grouping is enabled or not */
    void setDatasetGroupingEnabled(const bool& datasetGroupingEnabled) override;

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    void notifyDatasetAdded(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    void notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    void notifyDatasetRemoved(const QString& datasetGuid, const DataType& dataType) override;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    void notifyDatasetChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that dataset selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     * @param ignoreDatasets Pointer to datasets that should be ignored during notification
     */
    void notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets = nullptr) override;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    void notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName) override;

    /**
     * Notify all listeners that a dataset is locked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetLocked(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify all listeners that a dataset is unlocked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Register an event listener
     * @param eventListener Pointer to event listener to register
     */
    void registerEventListener(EventListener* eventListener) override;

    /**
     * Unregister an event listener
     * @param eventListener Pointer to event listener to unregister
     */
    void unregisterEventListener(EventListener* eventListener) override;

public:

    /** Initializes the core; loads all plugins from the designated plugin directory */
    void init();

    /** Adds the given plugin to the list of plugins kept by the core */
    void addPlugin(plugin::Plugin* plugin);

public: // Managers
    
    AbstractActionsManager& getActionsManager() override;
    AbstractPluginManager& getPluginManager() override;
    AbstractDataManager& getDataManager() override;
    AbstractDataHierarchyManager& getDataHierarchyManager() override;
    AbstractWorkspaceManager& getWorkspaceManager() override;
    AbstractProjectManager& getProjectManager();
    AbstractSettingsManager& getSettingsManager();

private:
    QScopedPointer<ActionsManager>          _actionsManager;            /** Actions manager for storing actions */
    QScopedPointer<PluginManager>           _pluginManager;             /** Plugin manager responsible for loading plug-ins and adding them to the core */
    QScopedPointer<DataManager>             _dataManager;               /** Data manager responsible for storing data sets and data selections */
    QScopedPointer<DataHierarchyManager>    _dataHierarchyManager;      /** Data hierarchy manager for providing a hierarchical dataset structure */
    QScopedPointer<WorkspaceManager>        _workspaceManager;          /** Workspace manager for controlling widgets layout */
    QScopedPointer<ProjectManager>          _projectManager;            /** Manager for loading/saving projects */
    QScopedPointer<SettingsManager>         _settingsManager;           /** Manager for managing global settings */
    std::vector<EventListener*>             _eventListeners;            /** List of classes listening for core events */

    friend class DataHierarchyManager;
};

}
