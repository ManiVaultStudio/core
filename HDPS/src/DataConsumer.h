#ifndef HDPS_PLUGIN_DATA_CONSUMER_H
#define HDPS_PLUGIN_DATA_CONSUMER_H

#include "RawData.h"

#include <QStringList>

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

    /**
     * Callback function which gets triggered when a selection changed.
     */
    virtual void selectionChanged(const QString dataName) = 0;

    /**
     * Returns a list of kinds of data that this data consumer can process.
     */
    virtual QStringList supportedDataKinds() = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataConsumer, "cytosplore.DataConsumer")

#endif // HDPS_PLUGIN_DATA_CONSUMER_H
