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
        PluginAdded,
        PluginRemoved
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

    class DataRenamedEvent : public DataEvent
    {
    public:
        DataRenamedEvent() : DataEvent(EventType::DataRenamed) {}

        QString oldName;
    };

    namespace plugin {
        class Plugin;
    }

    class PluginEvent : public HdpsEvent
    {
    public:
        PluginEvent(EventType type) :
            HdpsEvent(type),
            _plugin(nullptr)
        {
        }

        plugin::Plugin* _plugin;
    };

    class PluginAddedEvent : public PluginEvent
    {
    public:
        PluginAddedEvent() :
            PluginEvent(EventType::PluginAdded)
        {
        }

        QString     _inputDataset;  
    };

    class PluginRemovedEvent : public PluginEvent
    {
    public:
        PluginRemovedEvent() :
            PluginEvent(EventType::PluginRemoved)
        {
        }
    };
}

#endif // HDPS_EVENT_H
