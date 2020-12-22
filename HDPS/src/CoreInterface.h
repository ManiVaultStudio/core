#ifndef HDPS_COREINTERFACE_H
#define HDPS_COREINTERFACE_H

#include <QString>

namespace hdps
{
    class DataSet;
    class RawData;

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
     * Removes a RawData object and all of the data sets associated with this data.
     * Other data objects derived from this data object are converted to non-derived data.
     * Notifies all plug-ins of the removed data sets automatically.
     */
    //virtual void removeData(const QString dataName) = 0;

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

    template <class SetType>
    SetType& requestData(const QString name)
    {
        return dynamic_cast<SetType&>(requestData(name));
    }

    /** Notify all data consumers that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString name) = 0;
    /** Notify all data consumers that a dataset has been changed. */
    virtual void notifyDataChanged(const QString name) = 0;
    /** Notify all data consumers that a dataset has been removed. */
    virtual void notifyDataRemoved(const QString name) = 0;
    /** Notify all data consumers that a selection has changed. */
    virtual void notifySelectionChanged(const QString dataName) = 0;

protected:

    /**
     * Requests an instance of a data type plugin from the core which has the same
     * unique name as the given parameter. If no such instance can be found a fatal
     * error is thrown.
     */
    virtual RawData& requestRawData(const QString name) = 0;

    friend class RawData;
    friend class DataSet;
};

} // namespace hdps

#endif // HDPS_COREINTERFACE_H
