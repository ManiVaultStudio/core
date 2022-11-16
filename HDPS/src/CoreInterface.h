#pragma once

#include "PluginType.h"
#include "Dataset.h"

#include <actions/WidgetAction.h>

#include <QString>
#include <QSharedPointer>

#include <vector>
#include <functional>

namespace hdps
{
    class DatasetImpl;
    class DataType;
    class EventListener;
    class DataHierarchyItem;

    class AbstractPluginManager;
    class AbstractDataManager;
    class AbstractDataHierarchyManager;
    class AbstractActionsManager;

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

        using PluginTriggerActions = QVector<PluginTriggerAction*>;

        class AbstractLayoutManager;
    }

class CoreInterface : public hdps::gui::WidgetAction
{
public:
    //CoreInterface() = delete;

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

    /** Removes all currently loaded datasets */
    virtual void removeAllDatasets() = 0;

    /**
     * Copies a dataset and adds it to the data hierarchy
     * @param dataset Smart pointer to dataset to copy
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @return Smart pointer to the copied dataset
     */
    virtual Dataset<DatasetImpl> copyDataset(const Dataset<DatasetImpl>& dataset, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) = 0;

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

public: // Plugin creation

    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer to created plugin
     */
    virtual plugin::Plugin* requestPlugin(const QString& kind, Datasets datasets = Datasets()) = 0;

    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
     * @return Pointer to created plugin
     */
    template<typename PluginType>
    PluginType* requestPlugin(const QString& kind, Datasets datasets = Datasets())
    {
        return dynamic_cast<PluginType*>(requestPlugin(kind, datasets));
    }

public: // Plugin queries

    /**
     * Get plugins by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Pointers to plugins
     */
    virtual QVector<plugin::Plugin*> getPluginsByType(const plugin::Types& pluginTypes) const = 0;

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    virtual QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const = 0;

    /**
     * Get plugin trigger actions by \p pluginType and \p datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const Datasets& datasets) const = 0;

    /**
     * Get plugin trigger actions by \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const = 0;

    /**
     * Get plugin trigger actions by \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const = 0;

    /**
     * Get plugin trigger actions by \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    virtual gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const = 0;

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @return GUI name of the plugin, empty if the plugin kind was not found
     */
    virtual QString getPluginGuiName(const QString& pluginKind) const = 0;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon if the plugin kind was not found
     */
    virtual QIcon getPluginIcon(const QString& pluginKind) const = 0;

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

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    virtual void notifyDatasetAdded(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    virtual void notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    virtual void notifyDatasetRemoved(const QString& datasetGuid, const DataType& dataType) = 0;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    virtual void notifyDatasetChanged(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that dataset selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     * @param ignoreDatasets Pointer to datasets that should be ignored during notification
     */
    virtual void notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets = nullptr) = 0;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    virtual void notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName) = 0;

    /**
     * Notify all listeners that a dataset is locked
     * @param dataset Smart pointer to the dataset
     */
    virtual void notifyDatasetLocked(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify all listeners that a dataset is unlocked
     * @param dataset Smart pointer to the dataset
     */
    virtual void notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Register an event listener
     * @param eventListener Pointer to event listener to register
     */
    virtual void registerEventListener(EventListener* eventListener) = 0;

    /**
     * Unregister an event listener
     * @param eventListener Pointer to event listener to unregister
     */
    virtual void unregisterEventListener(EventListener* eventListener) = 0;

public: // Managers

    /** Get a reference to the plugin manager */
    virtual AbstractPluginManager& getPluginManager() = 0;

    /** Get a reference to the data manager */
    virtual AbstractDataManager& getDataManager() = 0;

    /** Get a reference to the data hierarchy manager */
    virtual AbstractDataHierarchyManager& getDataHierarchyManager() = 0;

    /** Get a reference to the layout manager */
    virtual gui::AbstractLayoutManager& getLayoutManager() = 0;

    /** Get a reference to the actions manager */
    virtual AbstractActionsManager& getActionsManager() = 0;
    
protected:
    bool    _datasetGroupingEnabled;        /** Whether datasets can be grouped or not */

    friend class plugin::RawData;
    friend class DatasetImpl;
    friend class EventListener;
};

}
