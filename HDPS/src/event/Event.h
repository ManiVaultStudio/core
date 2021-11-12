#ifndef HDPS_EVENT_H
#define HDPS_EVENT_H

#include "DataType.h"

#include <QString>

namespace hdps
{
    class DataSet;

    enum class EventType
    {
        DataAdded,
        DataChanged,
        DataAboutToBeRemoved,
        DataRemoved,
        SelectionChanged,
        GuiNameChanged
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
         * @param dataset Pointer to dataset
         */
        DataEvent(const EventType& eventType, DataSet* dataset) :
            HdpsEvent(eventType),
            _dataset(dataset)
        {
            Q_ASSERT(_dataset != nullptr);
        }

        /** Get reference to the dataset */
        DataSet& getDataset() {
            return *_dataset;
        }

    protected:
        DataSet*  _dataset;   /** Pointer to dataset for which the event occurred */
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
         * @param dataset Pointer to dataset
         */
        DataAddedEvent(DataSet* dataset) :
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
         * @param dataset Pointer to dataset
         */
        DataChangedEvent(DataSet* dataset) :
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
         * @param dataset Pointer to dataset
         */
        DataAboutToBeRemovedEvent(DataSet* dataset) :
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
         * @param dataset Pointer to dataset
         * @param datasetId Globally unique identifier of the dataset that has been removed
         */
        DataRemovedEvent(DataSet* dataset, const QString& datasetId) :
            DataEvent(EventType::DataRemoved, nullptr),
            _datasetId(datasetId)
        {
        }

        /** Get the globally unique identifier of the dataset that has been removed */
        QString getDatasetId() const {
            return _datasetId;
        }

    protected:
        QString     _datasetId;     /** Globally unique identifier of the dataset that has been removed */
    };

    /**
     * Selection changed event class
     * Data event which is emitted by the core when data selection has changed
     */
    class SelectionChangedEvent : public DataEvent
    {
    public:

        /**
         * Constructor
         * @param dataset Pointer to dataset
         */
        SelectionChangedEvent(DataSet* dataset) :
            DataEvent(EventType::SelectionChanged, dataset)
        {
        }
    };

    /**
     * Gui name changed event class
     * Data event which is emitted by the core when a dataset GUI name changed
     */
    class GuiNameChangedEvent : public DataEvent
    {
    public:

        /**
         * Constructor
         * @param dataset Pointer to dataset
         * @param previousGuiName Previous GUI name
         */
        GuiNameChangedEvent(DataSet* dataset, const QString previousGuiName) :
            DataEvent(EventType::GuiNameChanged, dataset),
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
}

#endif // HDPS_EVENT_H
