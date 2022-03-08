#include "DatasetPrivate.h"

#include "event/Event.h"
#include "Application.h"

#include "Set.h"

namespace hdps
{

using namespace util;

DatasetPrivate::DatasetPrivate() :
    EventListener(),
    _datasetGuid(),
    _dataset(nullptr)
{
    // Register for data events from the core
    registerDatasetEvents();
}

DatasetPrivate::DatasetPrivate(const DatasetPrivate& other) :
    EventListener(),
    _datasetGuid(),
    _dataset(nullptr)
{
    // Register for data events from the core
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
        _dataset        = dataset;
        _datasetGuid    = _dataset->getGuid();

        // Inform others that the pointer to the dataset changed
        emit changed(_dataset);
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

        // Register for data events
        registerDataEvent([this](DataEvent* dataEvent) {
            switch (dataEvent->getType()) {

                // Data is about to be removed
                case EventType::DataAboutToBeRemoved:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Notify others that the dataset is about to be removed
                    emit dataAboutToBeRemoved();

                    break;
                }

                case EventType::DataRemoved:
                {
                    // Get the data remove event
                    const auto dataRemovedEvent = static_cast<DataRemovedEvent*>(dataEvent);

                    // Only process dataset that we reference
                    if (_datasetGuid != dataRemovedEvent->getDatasetGuid())
                        break;

                    // Reset the internal pointer
                    reset();

                    // And notify others that the dataset is removed
                    emit dataRemoved(_datasetGuid);
                }

                // Data contents changed
                case EventType::DataChanged:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Notify others that the dataset contents changed
                    emit dataChanged();

                    break;
                }

                // Data selection changed
                case EventType::DataSelectionChanged:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Notify others that the dataset selection changed
                    emit dataSelectionChanged();

                    break;
                }

                // Dataset GUI name changed
                case EventType::DataGuiNameChanged:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Get dataset GUI name changed event
                    auto datasetGuiNameChangedEvent = static_cast<hdps::DataGuiNameChangedEvent*>(dataEvent);

                    // Notify others of the data GUI name change
                    emit dataGuiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset()->getGuiName());

                    break;
                }

                // Dataset child was added
                case EventType::DataChildAdded:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Get data child added event
                    auto dataChildAddedEvent = static_cast<hdps::DataChildAddedEvent*>(dataEvent);

                    // Notify others that a data child was added
                    emit dataChildAdded(dataChildAddedEvent->getChildDataset());

                    break;
                }

                // Dataset child was removed
                case EventType::DataChildRemoved:
                {
                    // Only process dataset that we reference
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    // Get data child removed event
                    auto dataChildRemovedEvent = static_cast<hdps::DataChildRemovedEvent*>(dataEvent);

                    // Notify others that a data child was removed
                    emit dataChildRemoved(dataChildRemovedEvent->getChildDatasetGuid());

                    break;
                }

                case EventType::DataAdded:
                case EventType::DataLocked:
                case EventType::DataUnlocked:
                    break;
            }
        });
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to register to dataset events in smart pointer", e.what());
    }
    catch (...)
    {
        exceptionMessageBox("Unable to register to dataset events in smart pointer");
    }
}

}
