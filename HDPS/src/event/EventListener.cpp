#include "EventListener.h"

#include "CoreInterface.h"
#include "Event.h"
#include "Set.h"

#include "util/SmartDataset.h"

#include <unordered_map>

namespace hdps
{

CoreInterface* EventListener::_eventCore = nullptr;

void EventListener::setEventCore(CoreInterface* core)
{
    _eventCore = core;

    _eventCore->registerEventListener(this);
}

EventListener::~EventListener()
{
    _eventCore->unregisterEventListener(this);
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

void EventListener::onDataEvent(DataEvent* dataEvent)
{
    // Only process data events which are valid
    if (!dataEvent->getDataset().isValid())
        return;

    if (dataEvent->getType() == EventType::DataSelectionChanged)
    {
        // Fire events for linked datasets
        auto& baseDataSet1 = dataEvent->getDataset()->getSourceDataset<DatasetImpl>();

        QString dataName1 = baseDataSet1->getRawDataName();

        // Get all available dataset from the core
        auto allDataSets = _eventCore->requestAllDataSets();

        // Go through all datasets in the system and find datasets with the same source data
        for (auto& dataSet : allDataSets)
        {
            auto& baseDataSet2 = dataSet->getSourceDataset<DatasetImpl>();

            QString dataName2   = baseDataSet2->getRawDataName();

            // Fire selection events for datasets with the same source data as the original event
            if (dataName1 == dataName2)
            {
                DataEvent sourceDataEvent = *dataEvent;

                sourceDataEvent.setDataset(*dataSet.get());

                if (_dataEventHandlersById.find(sourceDataEvent.getDataset()->getGuid()) != _dataEventHandlersById.end())
                    _dataEventHandlersById[sourceDataEvent.getDataset()->getGuid()](&sourceDataEvent);

                if (_dataEventHandlersByType.find(sourceDataEvent.getDataset()->getDataType()) != _dataEventHandlersByType.end())
                    _dataEventHandlersByType[sourceDataEvent.getDataset()->getDataType()](&sourceDataEvent);
            }
        }
        return;
    }
        
    if (_dataEventHandlersById.find(dataEvent->getDataset()->getGuid()) != _dataEventHandlersById.end())
        _dataEventHandlersById[dataEvent->getDataset()->getGuid()](dataEvent);

    if (_dataEventHandlersByType.find(dataEvent->getDataset()->getDataType()) != _dataEventHandlersByType.end())
        _dataEventHandlersByType[dataEvent->getDataset()->getDataType()](dataEvent);

    for (auto dataEventHandler : _dataEventHandlers)
        dataEventHandler(dataEvent);
}

}
