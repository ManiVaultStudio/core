#pragma once

#include "PluginType.h"

#include "util/SmartDataset.h"

#include <QString>
#include <QSharedPointer>

#include <vector>
#include <functional>

namespace hdps
{
    class DatasetImpl;
    class DataType;
    class EventListener;
    class DataHierarchyManager;
    class DataHierarchyItem;

    namespace plugin
    {
        class Plugin;
        class RawData;
    }

    // Forward declare smart pointer for datasets
    namespace util
    {
        template<typename DatasetType>
        class SmartDataset;
    }

class CoreInterface
{

public: // Data access

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if not valid)
     * @return Smart pointer to the added dataset
     */
    virtual Dataset<DatasetImpl> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) = 0;

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (root if smart pointer is not valid)
     * @return Smart pointer to the added dataset
     */
    template <class DatasetType>
    Dataset<DatasetType> addDataset(const QString& kind, const QString& dataSetGuiName, const Dataset<DatasetImpl>& parentDataset = Dataset<DatasetImpl>()) {
        return Dataset<DatasetType>(addDataset(kind, dataSetGuiName, parentDataset).get<DatasetType>());
    }

    /**
     * Removes one or more datasets
     * Other datasets derived from this dataset are converted to non-derived data
     * Notifies all plug-ins of the removed dataset automatically
     * @param datasets Smart pointers to the datasets that need to be removed
     * @param recursively Remove datasets recursively
     */
    virtual void removeDatasets(const QVector<Dataset<DatasetImpl>> datasets, const bool& recursively = false) = 0;

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to root in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    virtual Dataset<DatasetImpl> createDerivedData(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset) = 0;

    /**
     * Creates a dataset derived from a source dataset.
     * @param guiName GUI name for the new dataset from the core
     * @param sourceDataset Smart pointer to the source dataset from which this dataset will be derived
     * @param parentDataset Smart pointer to the parent dataset in the data hierarchy (will attach to root in hierarchy if not valid)
     * @return Smart pointer to the created derived dataset
     */
    template <class DatasetType>
    Dataset<DatasetType> createDerivedData(const QString& guiName, const Dataset<DatasetImpl>& sourceDataset, const Dataset<DatasetImpl>& parentDataset) {
        return util::Dataset<DatasetType>(createDerivedData(guiName, sourceDataset, parentDataset).get<DatasetType>());
    };

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
    template<class DatasetType>
    Dataset<DatasetType> requestDataset(const QString& datasetGuid)
    {
        return dynamic_cast<DatasetType&>(requestDataset(datasetGuid));
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
    template<class DatasetType>
    Dataset<DatasetType> requestSelection(const QString& rawDataName)
    {
        return Dataset<DatasetType>(dynamic_cast<DatasetType*>(requestSelection(rawDataName).get()));
    }

public: // Analysis

    /**
     * Request an analysis plugin by its kind
     * @param kind Type of analysis
     * @return Reference to created plugin
     */
    virtual plugin::Plugin& requestAnalysis(const QString& kind) = 0;

    /**
     * Analyze a dataset
     * @param kind Type of analysis
     * @param dataSet Smart pointer to the dataset to analyze
     */
    virtual const void analyzeDataset(const QString& kind, Dataset<DatasetImpl>& dataSet) = 0;

public: // Import/export

    /**
     * Imports a dataset
     * @param kind Type of import plugin
     */
    virtual const void importDataset(const QString& kind) = 0;

    /**
     * Exports a dataset
     * @param kind Type of export plugin
     * @param dataSet Smart pointer to the dataset to export
     */
    virtual const void exportDataset(const QString& kind, Dataset<DatasetImpl>& dataSet) = 0;

public: // Plugin queries

    /**
     * Get a list of plugin kinds (names) given a plugin type and data type(s)
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Types of data that the plugin should be compatible with (data type ignored when empty)
     * @return List of compatible plugin kinds that can handle the data type
     */
    virtual QStringList getPluginKindsByPluginTypeAndDataTypes(const plugin::Type& pluginType, const QVector<DataType>& dataTypes = QVector<DataType>()) const = 0;

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @return GUI name of the plugin, empty if the plugin kind was not found
     */
    virtual QString getPluginGuiName(const QString& pluginKind) const = 0;

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon the plugin kind was not found
     */
    virtual QIcon getPluginIcon(const QString& pluginKind) const = 0;

public: // Data hierarchy

    /** Get a reference to the data hierarchy manager */
    virtual DataHierarchyManager& getDataHierarchyManager() = 0;

    /**
     * Get data hierarchy item by dataset GUID
     * @param datasetGuid Globally unique identifier of the dataset
     * @return Reference to data hierarchy item
     */
    virtual DataHierarchyItem& getDataHierarchyItem(const QString& datasetGuid) = 0;

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    virtual void notifyDataAdded(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    template<class DatasetType>
    void notifyDataAdded(const Dataset<DatasetType>& dataset) {
        notifyDataAdded(Dataset<DatasetImpl>(dataset.get<DatasetImpl>()));
    }

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    virtual void notifyDataAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    template<class DatasetType>
    void notifyDataAboutToBeRemoved(const Dataset<DatasetType>& dataset) {
        notifyDataAboutToBeRemoved(Dataset<DatasetImpl>(dataset.get<DatasetImpl>()));
    }

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    virtual void notifyDataRemoved(const QString& datasetGuid, const DataType& dataType) = 0;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    virtual void notifyDataChanged(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    template<class DatasetType>
    void notifyDataChanged(const Dataset<DatasetType>& dataset) {
        notifyDataChanged(Dataset<DatasetImpl>(dataset.get<DatasetImpl>()));
    }

    /**
     * Notify listeners that data selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     */
    virtual void notifyDataSelectionChanged(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that data selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     */
    template<class DatasetType>
    void notifyDataSelectionChanged(const Dataset<DatasetType>& dataset) {
        notifyDataSelectionChanged(Dataset<DatasetImpl>(dataset.get<DatasetImpl>()));
    }

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    virtual void notifyDataGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName) = 0;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    template<class DatasetType>
    void notifyDataGuiNameChanged(const Dataset<DatasetType>& dataset) {
        notifyDataGuiNameChanged(Dataset<DatasetImpl>(dataset.get<DatasetImpl>()));
    }

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

    friend class RawData;
    friend class DatasetImpl;
    friend class EventListener;
};

}
