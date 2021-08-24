#ifndef HDPS_COREINTERFACE_H
#define HDPS_COREINTERFACE_H

#include "event/Event.h"

#include "DataHierarchyItem.h"

#include <QString>
#include <vector>
#include <functional>

namespace hdps
{
    class DataSet;
    class DataType;
    class EventListener;
    class HierarchyDataItem;

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
     * Removes a Dataset. Other datasets derived from this dataset are 
     * converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     */
    virtual void removeDataset(const QString datasetName) = 0;

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
     * Exports a dataset
     * @param exportKind Type of export plugin
     * @param datasetName Name of the dataset to export
     */
    virtual const void exportDataset(const QString exportKind, const QString& datasetName) = 0;

    virtual plugin::Plugin& requestAnalysis(const QString name) = 0;

    /** Notify registered listeners that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString datasetName) = 0;
    /** Notify registered listeners that a dataset has been changed. */
    virtual void notifyDataChanged(const QString datasetName) = 0;

    /** Notify registered listeners that a selection has changed. */
    virtual void notifySelectionChanged(const QString datasetName) = 0;

    /** Notify all event listeners that a dataset has been renamed. */
    virtual void notifyDataRenamed(const QString oldName, const QString newName) = 0;

    /**
     * Get data hierarchy item by dataset name
     * @param datasetName Name of the dataset
     * @return Pointer to data hierarchy item
     */
    virtual DataHierarchyItem* getDataHierarchyItem(const QString& datasetName) = 0;

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

    virtual void registerEventListener(EventListener* eventListener) = 0;

    virtual void unregisterEventListener(EventListener* eventListener) = 0;

    friend class RawData;
    friend class DataSet;
    friend class EventListener;
};

} // namespace hdps

#endif // HDPS_COREINTERFACE_H
