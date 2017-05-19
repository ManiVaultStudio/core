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
    virtual plugin::DataTypePlugin* addData(const QString kind) = 0;

    virtual void notifyDataAdded(const plugin::DataTypePlugin& data) = 0;
    virtual void notifyDataChanged(const plugin::DataTypePlugin& data) = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
