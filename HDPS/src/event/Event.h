#pragma once

#include "DataType.h"
#include "Dataset.h"

#include <QString>

#include <stdexcept>

namespace hdps
{
class DatasetImpl;

// Forward declare smart pointer for datasets
namespace util
{
    template<typename DatasetType>
    class SmartDataset;
}

enum class EventType
{
    DataAdded,
    DataChanged,
    DataAboutToBeRemoved,
    DataRemoved,
    DataSelectionChanged,
    DataGuiNameChanged,
    DataChildAdded,
    DataChildRemoved,
    DataLocked,
    DataUnlocked
};

class HdpsEvent
{
public:
    HdpsEvent(EventType type) :
        _type(type)
    {

    }

    EventType getType() const
    {
        return _type;
    }

private:
    EventType _type;
};

/**
 * Data event class
 * Event container class for datasets
 */
class DataEvent : public HdpsEvent
{
public:

    /**
     * Constructor
     * @param eventType Type of event
     * @param dataset Smart pointer to the dataset
     */
    DataEvent(const EventType& eventType, const Dataset<DatasetImpl>& dataset) :
        HdpsEvent(eventType),
        _dataset(dataset)
    {
        setDataset(dataset);
    }

    /** Get reference to the dataset */
    Dataset<DatasetImpl> getDataset() {
        return _dataset;
    }

    /** Get reference to the dataset */
    template<typename DatasetType>
    Dataset<DatasetType> getDataset() {
        return Dataset<DatasetType>(_dataset.get<DatasetType>());
    }

    /**
     * Set reference to the dataset
     * @param dataset Reference to the dataset
     */
    void setDataset(const Dataset<DatasetImpl>& dataset) {
        //if (!dataset.isValid())
        //    throw std::runtime_error("Data event contains an invalid pointer to a dataset");

        _dataset = dataset;
    }

protected:
    Dataset<DatasetImpl>    _dataset;   /** Smart pointer to the dataset for which the event occurred */
};

/**
 * Data added event class
 * Data event which is emitted by the core when data is added
 */
class DataAddedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DataAddedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataAdded, dataset)
    {
    }

    /** Get the name of the parent dataset */
    QString getParentDatasetName() const {
        return _parentDatasetName;
    }

    /**
     * Set the name of the parent dataset
     * @param parentDatasetName Name of the parent dataset
     */
    void setParentDatasetName(const QString& parentDatasetName) {
        _parentDatasetName = parentDatasetName;
    }

    /** Get whether the dataset is visible in the data hierarchy */
    bool getVisible() const {
        return _visible;
    }

    /**
     * Set whether the dataset is visible in the data hierarchy
     * @param visible Whether the dataset is visible in the data hierarchy
     */
    void setVisible(const bool& visible) {
        _visible = visible;
    }

protected:
    QString     _parentDatasetName;     /** Name of the parent dataset */
    bool        _visible;
};

/**
 * Data changed event class
 * Data event which is emitted by the core when data has changed
 */
class DataChangedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DataChangedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataChanged, dataset)
    {
    }
};

/**
 * Data about to be removed event class
 * Data event which is emitted by the core when data is about to be removed
 */
class DataAboutToBeRemovedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smarty pointer to dataset
     */
    DataAboutToBeRemovedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataAboutToBeRemoved, dataset)
    {
    }
};

/**
 * Data removed event class
 * Data event which is emitted by the core when data has been removed
 */
class DataRemovedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param datasetGuid GUID of the dataset that has been removed
     */
    DataRemovedEvent(const Dataset<DatasetImpl>& dataset, const QString& datasetGuid) :
        DataEvent(EventType::DataRemoved, nullptr),
        _datasetGuid(datasetGuid)
    {
    }

    /** Get the GUID of the dataset that has been removed */
    QString getDatasetGuid() const {
        return _datasetGuid;
    }

protected:
    QString     _datasetGuid;     /** GUID of the dataset that has been removed */
};

/**
 * Data selection changed event class
 * Data event which is emitted by the core when data selection has changed
 */
class DataSelectionChangedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DataSelectionChangedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataSelectionChanged, dataset)
    {
    }
};

/**
 * Data GUI name changed event class
 * Data event which is emitted by the core when a dataset GUI name changed
 */
class DataGuiNameChangedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param previousGuiName Previous GUI name
     */
    DataGuiNameChangedEvent(const Dataset<DatasetImpl>& dataset, const QString previousGuiName) :
        DataEvent(EventType::DataGuiNameChanged, dataset),
        _previousGuiName(previousGuiName)
    {
    }

    /** Get the previous GUI name of the dataset */
    QString getPreviousGuiName() const {
        return _previousGuiName;
    }

protected:
    QString     _previousGuiName;      /** Previous GUI name of the dataset */
};

/**
 * Data child added event class
 * Data event which is emitted by the core when a child dataset is added to the dataset
 */
class DataChildAddedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param childDataset Smart pointer to the child dataset
     */
    DataChildAddedEvent(const Dataset<DatasetImpl>& dataset, const Dataset<DatasetImpl>& childDataset) :
        DataEvent(EventType::DataChildAdded, dataset),
        _childDataset(childDataset)
    {
    }

    /** Get smart pointer to the child dataset */
    Dataset<DatasetImpl> getChildDataset() const {
        return _childDataset;
    }

protected:
    Dataset<DatasetImpl>    _childDataset;      /** Smart pointer to child dataset */
};

/**
 * Data child removed event class
 * Data event which is emitted by the core when a child dataset is removed from the dataset
 */
class DataChildRemovedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param childDatasetGuid GUID of the child dataset
     */
    DataChildRemovedEvent(const Dataset<DatasetImpl>& dataset, const QString& childDatasetGuid) :
        DataEvent(EventType::DataChildRemoved, dataset),
        _childDatasetGuid(childDatasetGuid)
    {
    }

    /** Get GUID of the child dataset */
    QString getChildDatasetGuid() const {
        return _childDatasetGuid;
    }

protected:
    QString     _childDatasetGuid;      /** GUID of of the child dataset */
};

/**
 * Data locked event class
 * Data event which is emitted by the core when a dataset is locked
 */
class DataLockedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DataLockedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataLocked, dataset)
    {
    }
};

/**
 * Data unlocked event class
 * Data event which is emitted by the core when a dataset is locked
 */
class DataUnlockedEvent : public DataEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DataUnlockedEvent(const Dataset<DatasetImpl>& dataset) :
        DataEvent(EventType::DataUnlocked, dataset)
    {
    }
};

/**
 * Dataset grouping event class
 * Event container class for datasets grouping events
 */
class DatasetGroupingEvent : public HdpsEvent
{
public:

    /**
     * Constructor
     * @param eventType Type of event
     * @param datasetGroupingEnabled Whether datasets can be grouped or not
     */
    DatasetGroupingEvent(const EventType& eventType, const bool& datasetGroupingEnabled) :
        HdpsEvent(eventType),
        _datasetGroupingEnabled(datasetGroupingEnabled)
    {
    }

    /** Get whether dataset grouping is enabled or not */
    bool isDatasetGroupingEnabled() const
    {
        return _datasetGroupingEnabled;
    }

    /** Get whether dataset grouping is enabled or not */
    void setDatasetGroupingEnabled(const bool& datasetGroupingEnabled)
    {
        if (datasetGroupingEnabled == _datasetGroupingEnabled)
            return;

        _datasetGroupingEnabled = datasetGroupingEnabled;
    }

protected:
    bool    _datasetGroupingEnabled;   /** Whether datasets can be grouped or not */
};

}
