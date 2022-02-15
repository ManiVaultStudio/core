#pragma once

#include "DataType.h"

#include <unordered_map>
#include <functional>
#include <vector>

namespace hdps
{

class CoreInterface;
class DataEvent;

class EventListener
{
public:

    /** Event registration function signatures */
    using DataEventHandler = std::function<void(DataEvent*)>;

    // Disabled until a better solution can be found as currently it requires
    // plugins to manually unregister if they are no longer interested in a dataset.
    //void registerDataEventByName(QString dataSetName, DataEventHandler callback);
    void registerDataEventByType(DataType dataType, DataEventHandler callback);
    void registerDataEvent(DataEventHandler callback);

protected:

    /**
     * Sets internal static event core, called once upon plugin creation
     * @param core Pointer to the core
     */
    void setEventCore(CoreInterface* core);

    /** Destructor, unregisters the event listener from the core */
    ~EventListener();

private:

    /**
     * Invoked when a data event occurs
     * @param dataEvent Pointer to data event that occurred
     */
    void onDataEvent(DataEvent* dataEvent);

    std::unordered_map<QString, DataEventHandler>   _dataEventHandlersById;         /** Data event handlers by dataset globally unique identifier */
    std::unordered_map<DataType, DataEventHandler>  _dataEventHandlersByType;       /** Data event handlers by data type */
    std::vector<DataEventHandler>                   _dataEventHandlers;             /** Non-specific Data event handlers */

    /** Pointer to the the (event) core */
    static CoreInterface* _eventCore;

    friend class Core;
};

}
