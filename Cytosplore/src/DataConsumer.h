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
    * @param name - The name of the dataset which was created.
    */
    virtual void dataAdded(const QString name) = 0;

    /**
    * Callback function which gets triggered when a dataset is changed.
    * @param name - The name of the dataset which was changed.
    */
    virtual void dataChanged(const QString name) = 0;

    /**
    * Callback function which gets triggered when a dataset is removed.
    */
    virtual void dataRemoved(const QString name) = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataConsumer, "cytosplore.DataConsumer")

#endif // HDPS_PLUGIN_DATA_CONSUMER_H
