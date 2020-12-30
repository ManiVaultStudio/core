#ifndef HDPS_COREINTERFACE_H
#define HDPS_COREINTERFACE_H

#include <QString>
#include <vector>
#include <functional>

namespace hdps
{
    class DataSet;
    class RawData;
    class DataType;

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
     * Removes a Dataset. Other datasets derived from this dataset are converted to non-derived data.
     * Notifies all plug-ins of the removed dataset automatically.
     */
    virtual void removeDataset(const QString dataName) = 0;

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
     * Request a list of all datasets in the core
     */
    virtual const std::vector<const DataSet*> requestDatasets() = 0;

    template <class SetType>
    SetType& requestData(const QString name)
    {
        return dynamic_cast<SetType&>(requestData(name));
    }


    /** Register for data added events */
    using DataAddedFunction = std::function<void(QString)>;//void (*)(QString);
    using DataChangedFunction = std::function<void(QString)>;//void (*)(QString);
    using DataRemovedFunction = std::function<void(QString)>;
    using SelectionChangedFunction = std::function<void(QString)>;

    virtual void registerDatasetChanged(QString datasetName, DataChangedFunction func) = 0;
    virtual void registerSelectionChanged(QString datasetName, SelectionChangedFunction func) = 0;

    virtual void registerDataTypeAdded(DataType dataType, DataAddedFunction func) = 0;
    virtual void registerDataTypeChanged(DataType dataType, DataChangedFunction func) = 0;
    virtual void registerDataTypeRemoved(DataType dataType, DataRemovedFunction func) = 0;

    /** Notify all data consumers that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString name) = 0;
    /** Notify all data consumers that a dataset has been changed. */
    virtual void notifyDataChanged(const QString name) = 0;

    /** Notify all data consumers that a selection has changed. */
    virtual void notifySelectionChanged(const QString dataName) = 0;

protected:

    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    virtual RawData& requestRawData(const QString name) = 0;

    /**
    * Request a selection from the data manager by its corresponding raw data name.
    */
    virtual DataSet& requestSelection(const QString rawdataName) = 0;

    friend class RawData;
    friend class DataSet;
};

} // namespace hdps

#endif // HDPS_COREINTERFACE_H
