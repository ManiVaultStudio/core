// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataType.h"

#include <QSet>

#include <unordered_map>
#include <functional>
#include <vector>

namespace hdps
{

class CoreInterface;
class DatasetEvent;

class EventListener final
{
public:

    /** Event registration function signatures */
    using DataEventHandler = std::function<void(DatasetEvent*)>;

    // Disabled until a better solution can be found as currently it requires
    // plugins to manually unregister if they are no longer interested in a dataset.
    //void registerDataEventByName(QString dataSetName, DataEventHandler callback);
    void registerDataEventByType(DataType dataType, DataEventHandler callback);
    void registerDataEvent(DataEventHandler callback);

public:

    /** Constructor, registers the event listener */
    EventListener();

    /** Destructor, unregisters the event listener from the core */
    virtual ~EventListener();

public: // Event filtering

    /**
     * Determines whether the listener should listen to a specific event type or not
     * @param eventType Type of event
     * @return Boolean indicating whether the listener should listen to a specific event type or not
     */
    bool isEventTypeSupported(std::uint32_t eventType) const;

    /**
     * Add supported event type
     * @param eventType Type of event that should be supported
     */
    void addSupportedEventType(std::uint32_t eventType);

    /**
     * Remove supported event type
     * @param eventType Type of event that should be not be supported anymore
     */
    void removeSupportedEventType(std::uint32_t eventType);

    /**
     * Add supported event types
     * @param eventType Types of events that should be supported
     */
    void setSupportedEventTypes(const QSet<std::uint32_t>& eventTypes);

private:

    /**
     * Invoked when a data event occurs
     * @param dataEvent Pointer to data event that occurred
     */
    void onDataEvent(DatasetEvent* dataEvent);

    std::unordered_map<QString, DataEventHandler>   _dataEventHandlersById;         /** Data event handlers by dataset globally unique identifier */
    std::unordered_map<DataType, DataEventHandler>  _dataEventHandlersByType;       /** Data event handlers by data type */
    std::vector<DataEventHandler>                   _dataEventHandlers;             /** Non-specific Data event handlers */
    QSet<std::uint32_t>                             _supportEventTypes;             /** Event types this listener should listen to (will listen to all if left empty) */

    friend class AbstractEventManager;
};

}
