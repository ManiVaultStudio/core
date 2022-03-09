#include "EventListener.h"

#include "CoreInterface.h"
#include "Event.h"
#include "Set.h"

#include "Dataset.h"

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
    if (_eventCore)
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
    if (dataEvent->getType() == EventType::DataRemoved) {

        // Get the data remove event
        const auto dataRemovedEvent = static_cast<DataRemovedEvent*>(dataEvent);

        // Find data handlers with identical GUID and call them
        if (_dataEventHandlersById.find(dataRemovedEvent->getDatasetGuid()) != _dataEventHandlersById.end())
            _dataEventHandlersById[dataRemovedEvent->getDatasetGuid()](dataEvent);

        // Find data handlers with the same type and call them
        if (_dataEventHandlersByType.find(dataRemovedEvent->getDataType()) != _dataEventHandlersByType.end())
            _dataEventHandlersByType[dataRemovedEvent->getDataType()](dataEvent);

        // Call all data handlers
        for (auto dataEventHandler : _dataEventHandlers)
            dataEventHandler(dataEvent);
    }

    // Only process data events which are valid
    if (!dataEvent->getDataset().isValid())
        return;

    // Data selection changes need special treatment
    if (dataEvent->getType() == EventType::DataSelectionChanged)
    {
        // Get smart pointer to the source dataset of which the selection changed
        auto eventSourceDataset = dataEvent->getDataset()->getSourceDataset<DatasetImpl>();

        // Get name of the raw data of the event dataset
        const auto eventSourceDatasetRawName = eventSourceDataset->getRawDataName();

        // Get all available dataset from the core
        const auto allDatasets = _eventCore->requestAllDataSets();

        // Go through all datasets in the system and find datasets with the same source data
        for (auto& candidateDataset : allDatasets)
        {
            // Get source of the candidate dataset
            auto candidateSourceDataset = candidateDataset->getSourceDataset<DatasetImpl>();

            // Get name of the raw data of the candidate dataset
            const auto candidateRawDataName = candidateSourceDataset->getRawDataName();

            // Fire selection events for datasets with the same source data as the original event
            if (eventSourceDatasetRawName == candidateRawDataName)
            {
                // Create a source data event
                auto sourceDataEvent = *dataEvent;

                // Set the correct dataset
                sourceDataEvent.setDataset(candidateDataset);

                // Find data handlers with identical GUID and call them
                if (_dataEventHandlersById.find(sourceDataEvent.getDataset()->getGuid()) != _dataEventHandlersById.end())
                    _dataEventHandlersById[sourceDataEvent.getDataset()->getGuid()](&sourceDataEvent);

                // Find data handlers with the same type and call them
                if (_dataEventHandlersByType.find(sourceDataEvent.getDataset()->getDataType()) != _dataEventHandlersByType.end())
                    _dataEventHandlersByType[sourceDataEvent.getDataset()->getDataType()](&sourceDataEvent);

                // Call all data handlers
                for (auto dataEventHandler : _dataEventHandlers)
                    dataEventHandler(&sourceDataEvent);
            }
        }

        return;
    }

    // Find data handlers with identical GUID and call them
    if (_dataEventHandlersById.find(dataEvent->getDataset()->getGuid()) != _dataEventHandlersById.end())
        _dataEventHandlersById[dataEvent->getDataset()->getGuid()](dataEvent);

    // Find data handlers with the same type and call them
    if (_dataEventHandlersByType.find(dataEvent->getDataset()->getDataType()) != _dataEventHandlersByType.end())
        _dataEventHandlersByType[dataEvent->getDataset()->getDataType()](dataEvent);

    // Call all data handlers
    for (auto dataEventHandler : _dataEventHandlers)
        dataEventHandler(dataEvent);
}

}
