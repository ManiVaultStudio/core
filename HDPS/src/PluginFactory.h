#ifndef HDPS_PLUGINFACTORY_H
#define HDPS_PLUGINFACTORY_H

#include "Plugin.h"
#include "DataType.h"

#include <QObject>

namespace hdps
{
    class DataSet;

namespace plugin
{

class PluginFactory : public QObject
{
    Q_OBJECT

public:
    ~PluginFactory() override {};

    virtual Plugin* produce() = 0;

    /**
     * Returns a list of datatypes that are supported for operations by
     * the plugin produced in this factory.
     */
    virtual DataTypes supportedDataTypes() const = 0;

    //virtual bool isCompatible(DataSet& dataSet) = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::PluginFactory, "hdps.PluginFactory")

#endif // HDPS_PLUGINFACTORY_H
