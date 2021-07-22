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

void EventListener::registerAnalysisEvent(AnalysisEventHandler callback)
{
    _analysisEventHandlers.push_back(callback);
}

void EventListener::onDataEvent(DataEvent* dataEvent)
{
    if (dataEvent->getType() == EventType::SelectionChanged)
    {
        // Fire events for linked datasets
        DataSet& baseDataSet1 = DataSet::getSourceData(_eventCore->requestData(dataEvent->dataSetName));
        QString dataName1 = baseDataSet1.getDataName();

        const std::vector<QString>& dataSetNames = _eventCore->requestAllDataNames();

        // Go through all datasets in the system and find datasets with the same source data
        for (const QString& dataSetName : dataSetNames)
        {
            DataSet& baseDataSet2 = DataSet::getSourceData(_eventCore->requestData(dataSetName));
            QString dataName2 = baseDataSet2.getDataName();

            // Fire selection events for datasets with the same source data as the original event
            if (dataName1 == dataName2)
            {
                DataEvent sourceDataEvent = *dataEvent;
                sourceDataEvent.dataSetName = dataSetName;
                    
                if (_dataEventHandlersByName.find(sourceDataEvent.dataSetName) != _dataEventHandlersByName.end())
                    _dataEventHandlersByName[sourceDataEvent.dataSetName](&sourceDataEvent);

                if (_dataEventHandlersByType.find(sourceDataEvent.dataType) != _dataEventHandlersByType.end())
                    _dataEventHandlersByType[sourceDataEvent.dataType](&sourceDataEvent);
            }
        }
        return;
    }
        
    if (_dataEventHandlersByName.find(dataEvent->dataSetName) != _dataEventHandlersByName.end())
        _dataEventHandlersByName[dataEvent->dataSetName](dataEvent);

    if (_dataEventHandlersByType.find(dataEvent->dataType) != _dataEventHandlersByType.end())
        _dataEventHandlersByType[dataEvent->dataType](dataEvent);

    for (auto dataEventHandler : _dataEventHandlers)
        dataEventHandler(dataEvent);
}

void EventListener::onAnalysisEvent(const AnalysisEvent& analysisEvent)
{
    for (auto analysisEventHandler : _analysisEventHandlers)
        analysisEventHandler(analysisEvent);
}

}
