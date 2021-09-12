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

class CoreInterface
{
public:
    /**
     * Requests the plugin manager to create new RawData of the given kind.
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data.
     * @param parentDatasetName Name of the parent dataset in the data hierarchy
     */
    virtual const QString addData(const QString kind, const QString name, const QString& parentDatasetName = "") = 0;

    /**
     * Removes one or more datasets. Other datasets derived from this dataset are 
     * converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     * @param datasetNames Names of the datasets to remove
     * @param recursively Remove datasets recursively
     */
    virtual void removeDatasets(const QStringList& datasetNames, const bool& recursively = false) = 0;

    /**
     * Renames a dataset
     * @param currentDatasetName Current name of the dataset
     * @param intendedDatasetName Intended name of the dataset
     * @return New name of the dataset
     */
    virtual QString renameDataset(const QString& currentDatasetName, const QString& intendedDatasetName) = 0;

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
     * @param sourceSet Source dataset
     * @param newSetName Intended name of the subset
     * @param dataHierarchyParent Name of the parent in the data hierarchy (sourceDatasetName if is used if empty)
     * @param visible Whether the new dataset is visible in the user interface
     */
    virtual QString createSubsetFromSelection(const DataSet& selection, const DataSet& sourceSet, const QString newSetName, const QString dataHierarchyParent = "", const bool& visible = true) = 0;

    /**
     * Requests a dataset from the core which has the same unique name
     * as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    virtual DataSet& requestData(const QString name) = 0;

    /**
    * Request all data set names.
    */
    virtual std::vector<QString> requestAllDataNames() = 0;

    /**
    * Request names for data sets of a specific type.
    */
    virtual std::vector<QString> requestAllDataNames(const std::vector<DataType> dataTypes) = 0;

    template <class SetType>
    SetType& requestData(const QString name)
    {
        return dynamic_cast<SetType&>(requestData(name));
    }

    /**
     * Analyzes a dataset
     * @param analysisKind Type of analysis plugin
     * @param datasetName Name of the dataset to analyze
     */
    virtual const void analyzeDataset(const QString analysisKind, const QString& datasetName) = 0;

    /**
     * Imports a dataset
     * @param importKind Type of import plugin
     */
    virtual const void importDataset(const QString importKind) = 0;

    /**
     * Exports a dataset
     * @param exportKind Type of export plugin
     * @param datasetName Name of the dataset to export
     */
    virtual const void exportDataset(const QString exportKind, const QString& datasetName) = 0;

    virtual plugin::Plugin& requestAnalysis(const QString name) = 0;

    /**
     * Get data hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Pointer to data hierarchy item
     */
    virtual DataHierarchyItem* getDataHierarchyItem(const QString& datasetName) = 0;

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

    /** Get the data hierarchy manager */
    virtual DataHierarchyManager& getDataHierarchyManager() = 0;

public: // Events & notifications

    /**
     * Notify all data consumers that a new dataset has been added to the core
     * @param datasetName Name of the dataset that was added
     */
    virtual void notifyDataAdded(const QString datasetName) = 0;

    /**
     * Notify all data consumers that a dataset has been changed
     * @param datasetName Name of the dataset of which the data changed
     */
    virtual void notifyDataChanged(const QString datasetName) = 0;

    /**
     * Notify all data consumers that a selection has changed
     @param datasetName Name of the dataset of which the selection changed
     */
    virtual void notifySelectionChanged(const QString datasetName) = 0;

    /**
     * Notify all event listeners that a dataset has been renamed
     * @param oldName Old dataset name
     * @param newName New dataset name
     */
    virtual void notifyDataRenamed(const QString oldName, const QString newName) = 0;

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

protected:

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

    

    friend class RawData;
    friend class DataSet;
    friend class EventListener;
};

} // namespace hdps

#endif // HDPS_COREINTERFACE_H
