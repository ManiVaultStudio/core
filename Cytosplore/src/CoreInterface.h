#ifndef HDPS_CORE_INTERFACE_H
#define HDPS_CORE_INTERFACE_H

#include <QString>

namespace hdps
{
class Set;

namespace plugin
{
class DataTypePlugin;
}

class CoreInterface
{
public:
    /**
    * Create a new dataset instance of the given kind.
    */
    virtual const QString addData(const QString kind) = 0;

    /**
    * Request a plugin from the core by its name.
    */
    virtual plugin::DataTypePlugin* requestPlugin(const QString name) = 0;

    /**
    * Request a dataset from the core by its name.
    */
    virtual Set* requestData(const QString name) = 0;

    /** Notify all data consumers that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const QString name) = 0;
    /** Notify all data consumers that a dataset has been changed. */
    virtual void notifyDataChanged(const QString name) = 0;
    /** Notify all data consumers that a dataset has been removed. */
    virtual void notifyDataRemoved(const QString name) = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
