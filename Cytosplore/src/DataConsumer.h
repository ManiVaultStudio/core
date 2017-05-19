#ifndef HDPS_PLUGIN_DATA_CONSUMER_H
#define HDPS_PLUGIN_DATA_CONSUMER_H

#include "DataTypePlugin.h"

namespace hdps
{
namespace plugin
{

class DataConsumer
{
public:
    /**
    * Callback function which gets triggered when a new dataset is created.
    * @param data - The dataset which was created.
    */
    virtual void dataAdded(const DataTypePlugin& data) = 0;

    /**
    * Callback function which gets triggered when a dataset is changed.
    * @param data - The dataset which was changed.
    */
    virtual void dataChanged(const DataTypePlugin& data) = 0;

    /**
    * Callback function which gets triggered when a dataset is removed.
    */
    virtual void dataRemoved() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataConsumer, "cytosplore.DataConsumer")

#endif // HDPS_PLUGIN_DATA_CONSUMER_H
