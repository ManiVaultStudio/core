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
    virtual const plugin::DataTypePlugin* addData(const QString kind) = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
