// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <CoreInterface.h>
#include <PluginType.h>
#include <DataType.h>

#include "DataManager.h"
#include "PluginManager.h"
#include "ActionsManager.h"
#include "DataHierarchyManager.h"
#include "EventManager.h"
#include "WorkspaceManager.h"
#include "ProjectManager.h"
#include "SettingsManager.h"
#include "TaskManager.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace mv {

class Core : public CoreInterface
{
public:

    /** Default constructor */
    Core();

    /** Default destructor */
    ~Core();

public:

    /** Creates the core managers */
    void createManagers() override;

    /** Initializes the core */
    void initialize() override;

    /** Resets the entire core implementation */
    void reset() override;

public: // Data access

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @param id Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to the added dataset
     */
    Dataset<DatasetImpl> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& id = "") override;

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

public: // Dataset grouping

    /** Get whether dataset grouping is enabled or not */
    bool isDatasetGroupingEnabled() const override;

    /** Get whether dataset grouping is enabled or not */
    void setDatasetGroupingEnabled(const bool& datasetGroupingEnabled) override;

public:

    /** Adds the given plugin to the list of plugins kept by the core */
    void addPlugin(plugin::Plugin* plugin);

public: // Managers

    AbstractManager* getManager(const ManagerType& managerType) override;

    AbstractActionsManager& getActionsManager() override;
    AbstractPluginManager& getPluginManager() override;
    AbstractEventManager& getEventManager() override;
    AbstractDataManager& getDataManager() override;
    AbstractDataHierarchyManager& getDataHierarchyManager() override;
    AbstractTaskManager& getTaskManager() override;
    AbstractWorkspaceManager& getWorkspaceManager() override;
    AbstractProjectManager& getProjectManager() override;
    AbstractSettingsManager& getSettingsManager() override;

private:
    QVector<AbstractManager*>   _managers;      /** All managers in the core */
    
    friend class DataHierarchyManager;
};

}
