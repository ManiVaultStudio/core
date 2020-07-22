#ifndef HDPS_DATACONSUMER_H
#define HDPS_DATACONSUMER_H

#include "RawData.h"

#include <QList>

namespace hdps
{
    using DataTypes = QList<DataType>;

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
     * Returns a list of types of data that this data consumer can process.
     */
    virtual DataTypes supportedDataTypes() const = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::DataConsumer, "cytosplore.DataConsumer")

#endif // HDPS_DATACONSUMER_H
