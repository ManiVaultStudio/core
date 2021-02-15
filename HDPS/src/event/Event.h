#ifndef HDPS_EVENT_H
#define HDPS_EVENT_H

#include "DataType.h"

#include <QString>

namespace hdps
{
    enum class EventType
    {
        DataAdded,
        DataChanged,
        DataRemoved,
        SelectionChanged
    };

    class HdpsEvent
    {
    public:
        HdpsEvent(EventType type) :
            _type(type)
        {

        }

        EventType getType()
        {
            return _type;
        }

    private:
        EventType _type;
    };

    class DataEvent : public HdpsEvent
    {
    public:
        DataEvent(EventType type) :
            HdpsEvent(type),
            dataType("") // Empty type
        {

        }

        QString dataSetName;
        DataType dataType;
    };

    class DataAddedEvent : public DataEvent
    {
    public:
        DataAddedEvent() : DataEvent(EventType::DataAdded) {}
    };

    class DataChangedEvent : public DataEvent
    {
    public:
        DataChangedEvent() : DataEvent(EventType::DataChanged) {}
    };

    class DataRemovedEvent : public DataEvent
    {
    public:
        DataRemovedEvent() : DataEvent(EventType::DataRemoved) {}
    };

    class SelectionChangedEvent : public DataEvent
    {
    public:
        SelectionChangedEvent() : DataEvent(EventType::SelectionChanged) {}
    };
}

#endif // HDPS_EVENT_H
