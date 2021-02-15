#ifndef HDPS_EVENT_LISTENER_H
#define HDPS_EVENT_LISTENER_H

#include "Event.h"
#include "CoreInterface.h"

#include <unordered_map>

namespace hdps
{
    class EventListener
    {
    public:
        /** Event registration function signatures */
        using DataEventHandler = std::function<void(DataEvent)>;

        //void registerDataEventByName(QString dataSetName, DataEventHandler callback);

        void registerDataEventByType(DataType dataType, DataEventHandler callback);

    protected:
        void setEventCore(CoreInterface* core);

        ~EventListener();

    private:
        void onDataEvent(DataEvent& dataEvent);

        std::unordered_map<QString, DataEventHandler> _dataEventHandlersByName;
        std::unordered_map<DataType, DataEventHandler> _dataEventHandlersByType;

        static CoreInterface* _eventCore;

        friend class Core;
    };
}

#endif // HDPS_EVENT_LISTENER_H
