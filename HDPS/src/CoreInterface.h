#pragma once

#include "PluginType.h"
#include "Dataset.h"
#include "Application.h"

#include "AbstractActionsManager.h"
#include "AbstractPluginManager.h"
#include "AbstractDataManager.h"
#include "AbstractDataHierarchyManager.h"
#include "AbstractEventManager.h"
#include "AbstractWorkspaceManager.h"
#include "AbstractProjectManager.h"
#include "AbstractSettingsManager.h"

#include <QString>

#include <vector>
#include <functional>

namespace hdps
{
    class DatasetImpl;
    class DataType;
    class EventListener;
    class DataHierarchyItem;

    namespace plugin
    {
        class Plugin;
        class RawData;
        class LoaderPlugin;
        class WriterPlugin;
        class AnalysisPlugin;
        class ViewPlugin;
        class TransformationPlugin;
    }

    namespace gui
    {
        class PluginTriggerAction;
        class GlobalSettingsAction;

        using PluginTriggerActions = QVector<QPointer<PluginTriggerAction>>;
    }

class CoreInterface
{
public:

    /** Default constructor */
    CoreInterface() :
        _initialized(false),
        _datasetGroupingEnabled(false)
    {
    }
    
    /** Initializes all core managers */
    virtual void init() {
        _initialized = true;
    };

    /** Resets the entire core implementation */
    virtual void reset() = 0;

    /**
     * Get whether the core is initialized or not
     * @return Boolean determining whether the core is initialized or not
     */
    virtual bool isInitialized() const final {
        return _initialized;
    }

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
    virtual Dataset<DatasetImpl> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>(), const QString& guid = "") = 0;

    /**
     * Removes a single dataset
     * @param dataset Smart pointer to the dataset to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if smart pointer is not valid)
     * @return Smart pointer to the added dataset
     */
    template <class DatasetType>
    Dataset<DatasetType> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>())\
    {
        return Dataset<DatasetType>(addDataset(kind, dataSetGuiName, parentDataset).get<DatasetType>());
    }

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to source dataset in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    virtual Dataset<DatasetImpl> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) = 0;

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to root in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    template <typename DatasetType>
    Dataset<DatasetType> createDerivedDataset(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>())
    {
        return createDerivedDataset(guiName, sourceDataset, parentDataset);
    }

    /**
     * Creates a copy of the given selection set, adds the new set to the data manager and notifies all data consumers of the new set
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (sourceSetName if not valid)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Smart pointer to the created subset
     */
    virtual Dataset<DatasetImpl> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true) = 0;

    /**
     * Creates a copy of the given selection set, adds the new set to the data manager and notifies all data consumers of the new set
     * @param selection Smart pointer to the selection set
     * @param sourceDataset Smart pointer to the source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (sourceSetName if not valid)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Smart pointer to the created subset
     */
    template <typename DatasetType>
    Dataset<DatasetType> createSubsetFromSelection(const Dataset<DatasetImpl>& selection, const Dataset<DatasetImpl>& sourceDataset, const QString& guiName, const Dataset<DatasetImpl>& parentDataset, const bool& visible = true)
    {
        return createSubsetFromSelection(selection, sourceDataset, guiName, parentDataset, visible);
    }

    /**
     * Requests a dataset from the core by dataset GUID (if no such instance can be found a fatal error is thrown)
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    virtual Dataset<DatasetImpl> requestDataset(const QString& datasetGuid) = 0;

    /**
     * Requests a dataset of a specific type from the core by dataset GUID, if no such instance can be found a fatal error is thrown
     * @param datasetGuid GUID of the dataset
     * @return Smart pointer to the dataset
     */
    template<typename DatasetType>
    Dataset<DatasetType> requestDataset(const QString& datasetGuid)
    {
        return Dataset<DatasetType>(requestDataset(datasetGuid));
    }

    /**
     * Returns all data sets that are present in the core, filtered by data type(s)
     * Returns all data sets in case of an empty filter
     * @param dataTypes Data types to filter
     * @return Vector of references to datasets
     */
    virtual QVector<Dataset<DatasetImpl>> requestAllDataSets(const QVector<DataType>& dataTypes = QVector<DataType>()) = 0;

protected: // Data access

    /**
     * Requests an instance of a data type plugin from the core which has the same unique name as the given parameter, if no such instance can be found a fatal error is thrown
     * @param datasetName Name of the dataset
     */
    virtual plugin::RawData& requestRawData(const QString& datasetName) = 0;

    /**
     * Request a selection from the data manager by its corresponding raw data name.
     * @param rawdataName Name of the raw data
     */
    virtual Dataset<DatasetImpl> requestSelection(const QString& rawDataName) = 0;

    /**
     * Request a selection from the data manager by its corresponding raw data name.
     * @param rawdataName Name of the raw data
     */
    template<typename DatasetType>
    Dataset<DatasetType> requestSelection(const QString& rawDataName)
    {
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(requestSelection(rawDataName).get()));
    }

public: // Data grouping

    /**
     * Groups \p datasets into one dataset
     * @param datasets Two or more datasets to group
     * @param guiName Name of the created dataset in the GUI (if empty, the user will be prompted for a name)
     * @return Smart pointer to created group dataset
     */
    virtual Dataset<DatasetImpl> groupDatasets(const Datasets& datasets, const QString& guiName = "") = 0;

public: // Data hierarchy

    /**
     * Get data hierarchy item by dataset GUID
     * @param datasetGuid Globally unique identifier of the dataset
     * @return Reference to data hierarchy item
     */
    virtual DataHierarchyItem& getDataHierarchyItem(const QString& datasetGuid) = 0;

public: // Dataset grouping

    /** Get whether dataset grouping is enabled or not */
    virtual bool isDatasetGroupingEnabled() const = 0;

    /** Get whether dataset grouping is enabled or not */
    virtual void setDatasetGroupingEnabled(const bool& datasetGroupingEnabled) = 0;

public: // Managers
    
    virtual AbstractActionsManager& getActionsManager() = 0;
    virtual AbstractPluginManager& getPluginManager() = 0;
    virtual AbstractEventManager& getEventManager() = 0;
    virtual AbstractDataManager& getDataManager() = 0;
    virtual AbstractDataHierarchyManager& getDataHierarchyManager() = 0;
    virtual AbstractWorkspaceManager& getWorkspaceManager() = 0;
    virtual AbstractProjectManager& getProjectManager() = 0;
    virtual AbstractSettingsManager& getSettingsManager() = 0;

protected:
    bool    _initialized;               /** Boolean determining whether the core is initialized or not */
    bool    _datasetGroupingEnabled;    /** Whether datasets can be grouped or not */

    friend class plugin::RawData;
    friend class DatasetImpl;
    friend class EventListener;
};

/**
 * Convenience function to obtain access to the core
 * @return Pointer to the current instance of the core
 */
static CoreInterface* core() {
    return Application::core();
}

/**
 * Convenience function to obtain access to the actions manager in the core
 * @return Reference to abstract actions manager
 */
static AbstractActionsManager& actions() {
    return core()->getActionsManager();
}

/**
 * Convenience function to obtain access to the plugin manager in the core
 * @return Reference to abstract plugin manager
 */
static AbstractPluginManager& plugins() {
    return core()->getPluginManager();
}

/**
 * Convenience function to obtain access to the event manager in the core
 * @return Reference to abstract event manager
 */
static AbstractEventManager& events() {
    return core()->getEventManager();
}

/**
 * Convenience function to obtain access to the data manager in the core
 * @return Reference to abstract data manager
 */
static AbstractDataManager& data() {
    return core()->getDataManager();
}

/**
 * Convenience function to obtain access to the data hierarchy manager in the core
 * @return Reference to abstract data hierarchy manager
 */
static AbstractDataHierarchyManager& dataHierarchy() {
    return core()->getDataHierarchyManager();
}

/**
 * Convenience function to obtain access to the workspace manager in the core
 * @return Reference to abstract workspace manager
 */
static AbstractWorkspaceManager& workspaces() {
    return core()->getWorkspaceManager();
}

/**
 * Convenience function to obtain access to the project manager in the core
 * @return Reference to abstract project manager
 */
static AbstractProjectManager& projects() {
    return core()->getProjectManager();
}

/**
 * Convenience function to obtain access to the settings manager in the core
 * @return Reference to abstract settings manager
 */
static AbstractSettingsManager& settings() {
    return core()->getSettingsManager();
}

}
