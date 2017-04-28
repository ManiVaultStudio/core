#ifndef HDPS_PLUGIN_DATA_CONSUMER_H
#define HDPS_PLUGIN_DATA_CONSUMER_H

#include "DataTypePlugin.h"

namespace hdps
{
namespace plugin
{

class DataConsumer
{
    virtual void dataAdded(const DataTypePlugin& data) = 0;
    virtual void dataChanged(const DataTypePlugin& data) = 0;
    virtual void dataRemoved() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataConsumer, "cytosplore.DataConsumer")

#endif // HDPS_PLUGIN_DATA_CONSUMER_H
