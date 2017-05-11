#ifndef HDPS_CORE_INTERFACE_H
#define HDPS_CORE_INTERFACE_H

#include <QString>

namespace hdps
{

class CoreInterface
{
public:
    virtual void addData(const QString kind) = 0;
};

} // namespace hdps

#endif // HDPS_CORE_INTERFACE_H
