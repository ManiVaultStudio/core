#ifndef HDPS_CORE_INTERFACE_H
#define HDPS_CORE_INTERFACE_H

#include <QString>

namespace hdps
{
namespace plugin
{
    class DataTypePlugin;
}

class Set;

class CoreInterface
{
public:
    /**
    * Create a new dataset instance of the given kind.
    */
    virtual const QString addData(const QString kind, const QString name) = 0;

    virtual const QString createDerivedData(const QString kind, const QString name, const QString sourceName) = 0;

    /**
     * Create a new dataset from the given selection.
     */
    virtual void createSubsetFromSelection(const Set* selection, const QString newSetName) = 0;

    /**
    * Request a plugin from the core by its name.
    */
    virtual plugin::DataTypePlugin& requestData(const QString name) = 0;

    /**
    * Request a selection from the data manager by its name.
    */
    virtual Set* requestSelection(const QString name) = 0;

    /**
    * Request a dataset from the core by its name.
    */
    virtual Set& requestSet(const QString name) = 0;

    /** Notify all data consumers that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString name) = 0;
    /** Notify all data consumers that a dataset has been changed. */
    virtual void notifyDataChanged(const QString name) = 0;
    /** Notify all data consumers that a dataset has been removed. */
    virtual void notifyDataRemoved(const QString name) = 0;
    /** Notify all data consumers that a selection has changed. */
    virtual void notifySelectionChanged(const QString dataName) = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
