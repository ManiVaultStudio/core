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
        using DataEventHandler      = std::function<void(DataEvent*)>;
        using AnalysisEventHandler  = std::function<void(AnalysisEvent*)>;  /** Analysis event handler */

        // Disabled until a better solution can be found as currently it requires
        // plugins to manually unregister if they are no longer interested in a dataset.
        //void registerDataEventByName(QString dataSetName, DataEventHandler callback);

        void registerDataEventByType(DataType dataType, DataEventHandler callback);
        void registerDataEvent(DataEventHandler callback);
        void registerAnalysisEvent(AnalysisEventHandler callback);

    protected:
        // Sets internal static event core, called once upon plugin creation
        void setEventCore(CoreInterface* core);

        // Unregisters the event listener from the core
        ~EventListener();

    private:
        void onDataEvent(DataEvent* dataEvent);
        void onAnalysisEvent(AnalysisEvent* analysisEvent);

        std::unordered_map<QString, DataEventHandler>   _dataEventHandlersByName;
        std::unordered_map<DataType, DataEventHandler>  _dataEventHandlersByType;
        std::vector<DataEventHandler>                   _dataEventHandlers;
        std::vector<AnalysisEventHandler>               _analysisEventHandlers;

        static CoreInterface* _eventCore;

        friend class Core;
    };
}

#endif // HDPS_EVENT_LISTENER_H
