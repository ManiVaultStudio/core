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
        bool        _visibleInGui;
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
        class AnalysisPlugin;
    }

    class AnalysisEvent : public HdpsEvent
    {
    public:
        AnalysisEvent(EventType type, plugin::AnalysisPlugin* analysisPlugin) :
            HdpsEvent(type),
            _analysisPlugin(analysisPlugin)
        {
        }

        const plugin::AnalysisPlugin* getAnalysisPlugin() const {
            return _analysisPlugin;
        }

    protected:
        plugin::AnalysisPlugin* _analysisPlugin;
    };

    class AnalysisAddedEvent : public AnalysisEvent
    {
    public:
        AnalysisAddedEvent(plugin::AnalysisPlugin* analysisPlugin) :
            AnalysisEvent(EventType::AnalysisAdded, analysisPlugin)
        {
        }
    };

    class AnalysisRemovedEvent : public AnalysisEvent
    {
    public:
        AnalysisRemovedEvent(plugin::AnalysisPlugin* analysisPlugin) :
            AnalysisEvent(EventType::AnalysisRemoved, analysisPlugin)
        {
        }
    };

    class AnalysisStartedEvent : public AnalysisEvent
    {
    public:
        AnalysisStartedEvent(plugin::AnalysisPlugin* analysisPlugin) :
            AnalysisEvent(EventType::AnalysisStarted, analysisPlugin)
        {
        }
    };

    class AnalysisProgressSectionEvent : public AnalysisEvent
    {
    public:
        AnalysisProgressSectionEvent(plugin::AnalysisPlugin* analysisPlugin, const QString& section) :
            AnalysisEvent(EventType::AnalysisProgressSection, analysisPlugin),
            _section(section)
        {
        }

        QString getSection() const {
            return _section;
        }

    protected:
        QString   _section;      /** Progress section */
    };

    class AnalysisProgressPercentageEvent : public AnalysisEvent
    {
    public:
        AnalysisProgressPercentageEvent(plugin::AnalysisPlugin* analysisPlugin, const float& percentage) :
            AnalysisEvent(EventType::AnalysisProgressPercentage, analysisPlugin),
            _percentage(percentage)
        {
        }

        float getPercentage() const {
            return _percentage;
        }

    protected:
        float   _percentage;      /** Normalized progress percentage [0, 1] */
    };

    class AnalysisFinishedEvent : public AnalysisEvent
    {
    public:
        AnalysisFinishedEvent(plugin::AnalysisPlugin* analysisPlugin) :
            AnalysisEvent(EventType::AnalysisFinished, analysisPlugin)
        {
        }
    };

    class AnalysisAbortedEvent : public AnalysisEvent
    {
    public:
        AnalysisAbortedEvent(plugin::AnalysisPlugin* analysisPlugin, const QString& reason) :
            AnalysisEvent(EventType::AnalysisAborted, analysisPlugin),
            _reason(reason)
        {
        }

        QString getReason() const {
            return _reason;
        }

    protected:
        QString _reason;    /** Reason for aborting the analysis */
    };

}

#endif // HDPS_EVENT_H
