#pragma once

#include "DatasetPrivate.h"

#include "event/Event.h"

#include "Set.h"

namespace hdps
{

using namespace util;

void DatasetPrivate::set(DatasetImpl* dataset)
{
    try
    {
        if (dataset == nullptr) {
            reset();
            return;
        }

        // Set event core in the event listener
        setEventCore(dataset->getCore());

        // No need to process the same dataset
        if (dataset == _dataset)
            return;

        // Try to cast the dataset pointer to the target dataset type
        _dataset = dataset;

        _datasetId = _dataset->getGuid();

        // Inform others that the pointer to the dataset changed
        _signals.notifyChanged(dynamic_cast<DatasetImpl*>(_dataset));

        // Register for dataset events
        registerDatasetEvents();
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

                    break;
                }

                // Reset the reference when the data is removed
                case EventType::DataRemoved:
                {
                    // Reset the reference
                    reset();

                    // Notify others that the dataset is removed
                    _signals.notifyDataRemoved(getDatasetGuid());

                    break;
                }

                // Data contents changed
                case EventType::DataChanged:
                {
                    // Notify others that the dataset contents changed
                    _signals.notifyDataChanged();

                    break;
                }

                // Dataset GUI name changed
                case EventType::DataGuiNameChanged:
                {
                    // Get dataset GUI name changed event
                    auto datasetGuiNameChangedEvent = static_cast<hdps::DataGuiNameChangedEvent*>(dataEvent);

                    // Notify others of the data GUI name change
                    _signals.notifyDataGuiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset()->getGuiName());

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
