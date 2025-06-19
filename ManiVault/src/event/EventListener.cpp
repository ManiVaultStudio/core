// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EventListener.h"

#include "CoreInterface.h"
#include "Event.h"
#include "Set.h"

#include "Dataset.h"

#include <unordered_map>

namespace mv
{

EventListener::EventListener()
{
    core()->getEventManager().registerEventListener(this);
}

EventListener::~EventListener()
{
    if (!core()->isAboutToBeDestroyed())
		core()->getEventManager().unregisterEventListener(this);
}

bool EventListener::isEventTypeSupported(std::uint32_t eventType) const
{
    return _supportEventTypes.contains(eventType);
}

void EventListener::addSupportedEventType(std::uint32_t eventType)
{
    _supportEventTypes << eventType;
}

void EventListener::removeSupportedEventType(std::uint32_t eventType)
{
    _supportEventTypes.remove(eventType);
}

void EventListener::setSupportedEventTypes(const QSet<std::uint32_t>& eventTypes)
{
    _supportEventTypes = eventTypes;
}

//void EventListener::registerDataEventByName(QString dataSetName, DataEventHandler callback)
//{
//    _dataEventHandlersByName[dataSetName] = callback;
//}

void EventListener::registerDataEventByType(DataType dataType, DataEventHandler callback)
{
    _dataEventHandlersByType[dataType] = callback;
}

void EventListener::registerDataEvent(DataEventHandler callback)
{
    _dataEventHandlers.push_back(callback);
}

void EventListener::onDataEvent(DatasetEvent* dataEvent)
{
    if (!isEventTypeSupported(static_cast<std::uint32_t>(dataEvent->getType())))
        return;

    if (dataEvent->getType() == EventType::DatasetRemoved) {
        const auto dataRemovedEvent = static_cast<DatasetRemovedEvent*>(dataEvent);

        if (_dataEventHandlersById.find(dataRemovedEvent->getDatasetGuid()) != _dataEventHandlersById.end())
            _dataEventHandlersById[dataRemovedEvent->getDatasetGuid()](dataEvent);

        if (_dataEventHandlersByType.find(dataRemovedEvent->getDataType()) != _dataEventHandlersByType.end())
            _dataEventHandlersByType[dataRemovedEvent->getDataType()](dataEvent);

        for (const auto& dataEventHandler : _dataEventHandlers)
            dataEventHandler(dataEvent);
    }

    if (!dataEvent->getDataset().isValid())
        return;

    if (_dataEventHandlersById.find(dataEvent->getDataset()->getId()) != _dataEventHandlersById.end())
        _dataEventHandlersById[dataEvent->getDataset()->getId()](dataEvent);

    if (_dataEventHandlersByType.find(dataEvent->getDataset()->getDataType()) != _dataEventHandlersByType.end())
        _dataEventHandlersByType[dataEvent->getDataset()->getDataType()](dataEvent);

    for (const auto& dataEventHandler : _dataEventHandlers)
        dataEventHandler(dataEvent);
}

}
