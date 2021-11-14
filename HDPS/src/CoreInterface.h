#ifndef HDPS_COREINTERFACE_H
#define HDPS_COREINTERFACE_H

#include "PluginType.h"

#include "event/Event.h"

#include <QString>
#include <QSharedPointer>

#include <vector>
#include <functional>

namespace hdps
{
    class DataSet;
    class DataType;
    class EventListener;
    class DataHierarchyManager;
    class DataHierarchyItem;

    using SharedDataHierarchyItem = QSharedPointer<DataHierarchyItem>;

    namespace plugin
    {
        class Plugin;
        class RawData;
    }

    namespace util
    {
        template<typename DatasetType>
        class DatasetRef;
    }

class CoreInterface
{

public: // Data access

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataSet Pointer to parent dataset in the data hierarchy (root if nullptr)
     * @return Reference to the added dataset
     */
    virtual DataSet& addData(const QString& kind, const QString& dataSetGuiName, DataSet* parentDataSet = nullptr) = 0;

    /**
     * Requests the plugin manager to create new RawData of the given kind
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data
     * @param kind Kind of plugin
     * @param datasetGuiName Name of the added dataset in the GUI
     * @param parentDataSet Pointer to parent dataset in the data hierarchy (root if nullptr)
     * @return Reference to the added dataset
     */
    template <class SetType>
    SetType& addData(const QString& kind, const QString& dataSetGuiName, DataSet* parentDataSet = nullptr) {
        return dynamic_cast<SetType&>(addData(kind, dataSetGuiName, parentDataSet));
    }

    /**
     * Removes one or more datasets
     * Other datasets derived from this dataset are  converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically
     * @param datasets Datasets to remove
     * @param recursively Remove datasets recursively
     */
    virtual void removeDatasets(const QVector<DataSet*> datasets, const bool& recursively = false) = 0;

    /**
     * Creates a dataset derived from a source dataset.
     * @param nameRequest Preferred name for the new dataset from the core (May be changed if not unique)
     * @param sourceDatasetName Name of the source dataset from which this dataset will be derived
     * @param dataHierarchyParent Name of the parent in the data hierarchy (sourceDatasetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     */
    virtual const QString createDerivedData(const QString& nameRequest, const QString& sourceDatasetName, const QString& dataHierarchyParent = "") = 0;

    /**
     * Creates a copy of the given selection set and gives it a unique name based
     * on the name given to this function. Then adds the new set to the data manager
     * and notifies all data consumers of the new set.
     * @param selection Selection set
     * @param sourceDataset Source dataset
     * @param guiName GUI name of the subset
     * @param parentDataset Pointer to the parent dataset in the data hierarchy (sourceSetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     * @return Subset
     */
    virtual DataSet& createSubsetFromSelection(const DataSet& selection, const DataSet& sourceDataset, const QString& guiName, const DataSet* parentDataset = nullptr, const bool& visible = true) = 0;

    /**
     * Requests a dataset from the core by dataset globally unique identifier (if no such instance can be found a fatal error is thrown)
     * @param dataSetId Globally unique identifier of the dataset
     * @return Reference to data set
     */
    virtual DataSet& requestData(const QString& dataSetId) = 0;

    /**
     * Requests a dataset of a specific type from the core by dataset globally unique identifier
     * If no such instance can be found a fatal error is thrown
     * @param dataSetId Globally unique identifier of the dataset
     * @return Reference to data set
     */
    template <class SetType>
    SetType& requestData(const QString& dataSetId)
    {
        return dynamic_cast<SetType&>(requestData(name));
    }

    /**
     * Returns all data sets that are present in the core, filtered by data type(s)
     * Returns all data sets in case of an empty filter
     * @param dataTypes Data types to filter
     * @return Vector of pointers to datasets
     */
    virtual QVector<DataSet*> requestAllDataSets(const QVector<DataType>& dataTypes = QVector<DataType>()) = 0;

protected: // Data access

    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    virtual plugin::RawData& requestRawData(const QString datasetName) = 0;

    /**
    * Request a selection from the data manager by its corresponding raw data name.
    */
    virtual DataSet& requestSelection(const QString rawdataName) = 0;

public: // Analysis

    /**
     * Request an analysis by its kind
     * @param kind Type of analysis
     * @return Reference to created plugin
     */
    virtual plugin::Plugin& requestAnalysis(const QString& kind) = 0;

    /**
     * Analyze a dataset
     * @param kind Type of analysis
     * @param dataSet Dataset to analyze
     */
    virtual const void analyzeDataset(const QString& kind, DataSet& dataSet) = 0;

public: // Import/export

    /**
     * Imports a dataset
     * @param kind Type of import plugin
     */
    virtual const void importDataset(const QString& kind) = 0;

    /**
     * Exports a dataset
     * @param kind Type of export plugin
     * @param dataSet Reference to dataset to export
     */
    virtual const void exportDataset(const QString& kind, DataSet& dataSet) = 0;

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
     * Get data hierarchy item by dataset globally unique identifier
     * @param dataSetId Globally unique identifier of the dataset
     * @return Reference to data hierarchy item
     */
    virtual DataHierarchyItem& getDataHierarchyItem(const QString& dataSetId) = 0;

public: // Events & notifications

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Reference to the dataset that was added
     */
    virtual void notifyDataAdded(const DataSet& dataset) = 0;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Reference to the dataset which is about to be removed
     */
    virtual void notifyDataAboutToBeRemoved(const DataSet& dataset) = 0;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetId Globally unique identifier of the dataset that was removed
     * @param dataType Type of the data
     */
    virtual void notifyDataRemoved(const QString& datasetId, const DataType& dataType) = 0;

    /**
     * Notify listeners that a dataset has changed
     * @param dataset Reference to the dataset of which the data changed
     */
    virtual void notifyDataChanged(const DataSet& dataset) = 0;

    /**
     * Notify listeners that data selection has changed
     * @param dataset Reference to the dataset of which the selection changed
     */
    virtual void notifyDataSelectionChanged(const DataSet& dataset) = 0;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Reference to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    virtual void notifyDataGuiNameChanged(const DataSet& dataset, const QString& previousGuiName) = 0;

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
    friend class DataSet;
    friend class EventListener;
};

} // namespace hdps

#endif // HDPS_COREINTERFACE_H
