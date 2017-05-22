#ifndef HDPS_CORE_INTERFACE_H
#define HDPS_CORE_INTERFACE_H

#include <QString>

namespace hdps
{

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
    virtual plugin::DataTypePlugin* addData(const QString kind) = 0;

    /** Notify all data consumers that a new dataset has been added to the core. */
    virtual void notifyDataAdded(const plugin::DataTypePlugin& data) = 0;
    /** Notify all data consumers that a dataset has been changed. */
    virtual void notifyDataChanged(const plugin::DataTypePlugin& data) = 0;
    /** Notify all data consumers that a dataset has been removed. */
    virtual void notifyDataRemoved() = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
