#include "EventListener.h"

#include "CoreInterface.h"
#include "Set.h"

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
    if (dataEvent->getType() == EventType::SelectionChanged)
    {
        // Fire events for linked datasets
        DataSet& baseDataSet1 = DataSet::getSourceData(dataEvent->getDataset());
        QString dataName1 = baseDataSet1.getDataName();

        const auto& allDataSets = _eventCore->requestAllDataSets();

        // Go through all datasets in the system and find datasets with the same source data
        for (const auto& dataSet : allDataSets)
        {
            auto& baseDataSet2 = DataSet::getSourceData(*dataSet);
            QString dataName2   = baseDataSet2.getDataName();

            // Fire selection events for datasets with the same source data as the original event
            if (dataName1 == dataName2)
            {
                DataEvent sourceDataEvent = *dataEvent;

                sourceDataEvent.setDataset(*dataSet);
                    
                if (_dataEventHandlersById.find(sourceDataEvent.getDataset().getId()) != _dataEventHandlersById.end())
                    _dataEventHandlersById[sourceDataEvent.getDataset().getId()](&sourceDataEvent);

                if (_dataEventHandlersByType.find(sourceDataEvent.getDataset().getDataType()) != _dataEventHandlersByType.end())
                    _dataEventHandlersByType[sourceDataEvent.getDataset().getDataType()](&sourceDataEvent);
            }
        }
        return;
    }
        
    if (_dataEventHandlersById.find(dataEvent->getDataset().getId()) != _dataEventHandlersById.end())
        _dataEventHandlersById[dataEvent->getDataset().getId()](dataEvent);

    if (_dataEventHandlersByType.find(dataEvent->getDataset().getDataType()) != _dataEventHandlersByType.end())
        _dataEventHandlersByType[dataEvent->getDataset().getDataType()](dataEvent);

    for (auto dataEventHandler : _dataEventHandlers)
        dataEventHandler(dataEvent);
}

}
