// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataType.h"
#include "Dataset.h"

#include <QString>

#include <stdexcept>

namespace mv
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
    DatasetAdded,
    DatasetDataChanged,
    DatasetDataDimensionsChanged,
    DatasetAboutToBeRemoved,
    DatasetRemoved,
    DatasetDataSelectionChanged,
    DatasetChildAdded,
    DatasetChildRemoved,
    DatasetLocked,
    DatasetUnlocked
};

class ManiVaultEvent
{
public:
    ManiVaultEvent(EventType type) :
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
 * Dataset event class
 * Event container class for datasets
 */
class DatasetEvent : public ManiVaultEvent
{
public:

    /**
     * Constructor
     * @param eventType Type of event
     * @param dataset Smart pointer to the dataset
     */
    DatasetEvent(const EventType& eventType, const Dataset<DatasetImpl>& dataset) :
        ManiVaultEvent(eventType),
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
 * Dataset added event class
 * Dataset event which is emitted by the core when data is added
 */
class DatasetAddedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetAddedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetAdded, dataset),
        _parentDatasetName(""),
        _visible(true)
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
 * Dataset data changed event class
 * Dataset event which is emitted by the core when dataset data has changed
 */
class DatasetDataChangedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetDataChangedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetDataChanged, dataset)
    {
    }
};

/**
 * Dataset data dimensions changed event class
 * Dataset event which is emitted by the core when dataset data dimensions change
 */
class DatasetDataDimensionsChangedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetDataDimensionsChangedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetDataDimensionsChanged, dataset)
    {
    }
};

/**
 * Dataset about to be removed event class
 * Dataset event which is emitted by the core when a dataset is about to be removed
 */
class DatasetAboutToBeRemovedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to dataset
     */
    DatasetAboutToBeRemovedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetAboutToBeRemoved, dataset)
    {
    }
};

/**
 * Dataset removed event class
 * Dataset event which is emitted by the core when a dataset has been removed
 */
class DatasetRemovedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param datasetGuid GUID of the dataset that has been removed
     */
    DatasetRemovedEvent(const Dataset<DatasetImpl>& dataset, const QString& datasetGuid, const DataType& dataType) :
        DatasetEvent(EventType::DatasetRemoved, nullptr),
        _datasetGuid(datasetGuid),
        _dataType(dataType)
    {
    }

    /** Get the GUID of the dataset that has been removed */
    QString getDatasetGuid() const {
        return _datasetGuid;
    }

    /** Get the data type of the dataset that has been removed */
    DataType getDataType() const {
        return _dataType;
    }

protected:
    QString     _datasetGuid;       /** GUID of the dataset that has been removed */
    DataType    _dataType;          /** Dataset type */
};

/**
 * Dataset data selection changed event class
 * Dataset event which is emitted by the core when dataset data selection has changed
 */
class DatasetDataSelectionChangedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetDataSelectionChangedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetDataSelectionChanged, dataset)
    {
    }
};

/**
 * Dataset child added event class
 * Dataset event which is emitted by the core when a child dataset is added to the dataset
 */
class DatasetChildAddedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param childDataset Smart pointer to the child dataset
     */
    DatasetChildAddedEvent(const Dataset<DatasetImpl>& dataset, const Dataset<DatasetImpl>& childDataset) :
        DatasetEvent(EventType::DatasetChildAdded, dataset),
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
 * Dataset child removed event class
 * Dataset event which is emitted by the core when a child dataset is removed from the dataset
 */
class DatasetChildRemovedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     * @param childDatasetGuid GUID of the child dataset
     */
    DatasetChildRemovedEvent(const Dataset<DatasetImpl>& dataset, const QString& childDatasetGuid) :
        DatasetEvent(EventType::DatasetChildRemoved, dataset),
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
 * Dataset locked event class
 * Dataset event which is emitted by the core when a dataset became locked
 */
class DatasetLockedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetLockedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetLocked, dataset)
    {
    }
};

/**
 * Dataset unlocked event class
 * Dataset event which is emitted by the core when a dataset became unlocked
 */
class DatasetUnlockedEvent : public DatasetEvent
{
public:

    /**
     * Constructor
     * @param dataset Smart pointer to the dataset
     */
    DatasetUnlockedEvent(const Dataset<DatasetImpl>& dataset) :
        DatasetEvent(EventType::DatasetUnlocked, dataset)
    {
    }
};

/**
 * Dataset grouping event class
 * Event container class for datasets grouping events
 */
class DatasetGroupingEvent : public ManiVaultEvent
{
public:

    /**
     * Constructor
     * @param eventType Type of event
     * @param datasetGroupingEnabled Whether datasets can be grouped or not
     */
    DatasetGroupingEvent(const EventType& eventType, const bool& datasetGroupingEnabled) :
        ManiVaultEvent(eventType),
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
