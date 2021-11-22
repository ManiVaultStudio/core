#pragma once

#include "DatasetPrivate.h"

#include "event/Event.h"
#include "Application.h"

#include "Set.h"

namespace hdps
{

using namespace util;

DatasetPrivate::DatasetPrivate() :
    EventListener(),
    _datasetId(),
    _dataset(nullptr),
    _signals()
{
    // Register for data events
    registerDatasetEvents();
}

DatasetPrivate::DatasetPrivate(const DatasetPrivate& other) :
    EventListener(),
    _datasetId(),
    _dataset(nullptr),
    _signals()
{
    // Register for data events
    registerDatasetEvents();
}

void DatasetPrivate::set(DatasetImpl* dataset)
{
    try
    {
        if (dataset == nullptr) {
            reset();
            return;
        }

        // No need to process the same dataset
        if (dataset == _dataset)
            return;

        // Set internal dataset pointer and dataset GUID
        _dataset    = dataset;
        _datasetId  = _dataset->getGuid();

        // Inform others that the pointer to the dataset changed
        emit changed(dynamic_cast<DatasetImpl*>(_dataset));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to assign dataset to dataset smart pointer", e.what());
    }
    catch (...)
    {
        exceptionMessageBox("Unable to assign dataset to dataset smart pointer");
    }
}

void DatasetPrivate::registerDatasetEvents()
{
    try
    {
        // Set the event core (necessary for listening to data events)
        setEventCore(Application::core());

        // Register for data removal events
        registerDataEvent([this](DataEvent* dataEvent) {

            // Only proceed if we have a valid data set
            if (getDatasetGuid().isEmpty())
                return;

            // Only process datasets that we reference
            if (dataEvent->getDataset()->getGuid() != getDatasetGuid())
                return;

            switch (dataEvent->getType()) {

                // Data is about to be removed
                case EventType::DataAboutToBeRemoved:
                {
                    // Notify others that the dataset is about to be removed
                    _signals.notifyDataAboutToBeRemoved();

                    emit dataAboutToBeRemoved();

                    break;
                }

                // Reset the reference when the data is removed
                case EventType::DataRemoved:
                {
                    // Reset the reference
                    reset();

                    // Notify others that the dataset is removed
                    _signals.notifyDataRemoved(getDatasetGuid());
                    
                    emit dataRemoved(getDatasetGuid());

                    break;
                }

                // Data contents changed
                case EventType::DataChanged:
                {
                    // Notify others that the dataset contents changed
                    _signals.notifyDataChanged();
                    
                    emit dataChanged();

                    break;
                }

                // Dataset GUI name changed
                case EventType::DataGuiNameChanged:
                {
                    // Get dataset GUI name changed event
                    auto datasetGuiNameChangedEvent = static_cast<hdps::DataGuiNameChangedEvent*>(dataEvent);

                    // Notify others of the data GUI name change
                    _signals.notifyDataGuiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset()->getGuiName());
                    
                    emit dataGuiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset()->getGuiName());

                    break;
                }
            }
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize dataset smart pointer", e.what());
    }
    catch (...)
    {
        exceptionMessageBox("Unable to initialize dataset smart pointer");
    }
}

}
