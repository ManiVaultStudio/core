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
        SelectionChanged,
        DataRenamed,
        AnalysisAdded,
        AnalysisRemoved,
        AnalysisStarted,
        AnalysisProgressSection,
        AnalysisProgressPercentage,
        AnalysisFinished,
        AnalysisAborted
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

        QString     _parentDatasetName;
        bool        _visible;
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

    class DataRenamedEvent : public DataEvent
    {
    public:
        DataRenamedEvent() : DataEvent(EventType::DataRenamed) {}

        QString oldName;
    };
}

#endif // HDPS_EVENT_H
