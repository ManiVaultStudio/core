#ifndef HDPS_COREINTERFACE_H
#define HDPS_COREINTERFACE_H

#include "event/Event.h"

#include <QString>
#include <vector>
#include <functional>

namespace hdps
{
    class DataSet;
    class RawData;
    class DataType;
    class EventListener;
    class HierarchyDataItem;

    namespace plugin
    {
        class Plugin;
    }

class CoreInterface
{
public:
    /**
     * Requests the plugin manager to create new RawData of the given kind.
     * The manager will add the raw data to the core and return the
     * unique name of the data set linked with the raw data.
     */
    virtual const QString addData(const QString kind, const QString name) = 0;

    /**
     * Removes a Dataset. Other datasets derived from this dataset are 
     * converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     */
    virtual void removeDataset(const QString datasetName) = 0;

    /**
     * Creates a dataset derived from a source dataset.
     * @param nameRequest Preferred name for the new dataset from the core (May be changed if not unique)
     * @param sourceDatasetName Name of the source dataset from which this dataset will be derived
     */
    virtual const QString createDerivedData(const QString nameRequest, const QString sourceDatasetName) = 0;

    /**
     * Creates a copy of the given selection set and gives it a unique name based
     * on the name given to this function. Then adds the new set to the data manager
     * and notifies all data consumers of the new set.
     */
    virtual QString createSubsetFromSelection(const DataSet& selection, const DataSet& parentSet, const QString newSetName) = 0;

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
     * Exports a dataset
     * @param exportKind Type of export plugin
     * @param datasetName Name of the dataset to export
     */
    virtual const void exportDataset(const QString exportKind, const QString& datasetName) = 0;

    /**
     * Analyzes a dataset
     * @param analysisKind Type of analysis plugin
     * @param datasetName Name of the dataset to analyze
     */
    virtual const void analyzeDataset(const QString analysisKind, const QString& datasetName) = 0;

    virtual plugin::Plugin& requestAnalysis(const QString name) = 0;

    /**
     * Get a list of plugin kinds (names) given a plugin type and data type
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataType Type of data that the plugin should be compatible with
     * @return List of compatible plugin kinds that can handle the data type
     */
    virtual std::vector<QString> requestPluginKindsByPluginTypeAndDataType(const QString& pluginType, const DataType& dataType) = 0;

    /** Notify registered listeners that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString datasetName) = 0;
    /** Notify registered listeners that a dataset has been changed. */
    virtual void notifyDataChanged(const QString datasetName) = 0;

    /** Notify registered listeners that a selection has changed. */
    virtual void notifySelectionChanged(const QString datasetName) = 0;

    /** Notify all event listeners that a dataset has been renamed. */
    virtual void notifyDataRenamed(const QString oldName, const QString newName) = 0;

    /** Notify all event listeners of an analysis event */
    virtual void notifyAnalysisEvent(const AnalysisEvent& analysisEvent) = 0;

protected:

    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    virtual RawData& requestRawData(const QString datasetName) = 0;

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
