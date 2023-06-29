#include "DatasetPrivate.h"
#include "CoreInterface.h"
#include "event/Event.h"
#include "Application.h"
#include "Set.h"

#include <QMetaMethod>

#ifdef _DEBUG
    #define DATASET_PRIVATE_VERBOSE
#endif

namespace hdps
{

using namespace util;

DatasetPrivate::DatasetPrivate() :
    QObject(),
    _datasetId(),
    _dataset(nullptr),
    _eventListener()
{
    registerDatasetEvents();
}

DatasetPrivate::DatasetPrivate(const DatasetPrivate& other) :
    _datasetId(),
    _dataset(nullptr),
    _eventListener()
{
    registerDatasetEvents();
}

QString DatasetPrivate::getDatasetId() const
{
    return _datasetId;
}

void DatasetPrivate::setDatasetId(const QString& datasetId)
{
    if (datasetId == _datasetId)
        return;

    _datasetId = datasetId;

    if (_datasetId.isEmpty()) {
        reset();
    }
    else {
        _dataset = getDataset();

        emit changed(_dataset);
    }
}

DatasetImpl* DatasetPrivate::getDataset()
{
    if (_dataset == nullptr && !_datasetId.isEmpty())
        _dataset = Application::core()->requestDataset(_datasetId).get();

    return _dataset;
}

const DatasetImpl* DatasetPrivate::getDataset() const
{
    return const_cast<DatasetPrivate*>(this)->getDataset();
}

void DatasetPrivate::setDataset(DatasetImpl* dataset)
{
    if (dataset == nullptr) {
        reset();
    }
    else {
        if (dataset == _dataset)
            return;

        _dataset        = dataset;
        _datasetId    = _dataset->getId();

        emit changed(_dataset);
    }
}

void DatasetPrivate::reset()
{
    _dataset    = nullptr;
    _datasetId  = "";
}

void DatasetPrivate::connectNotify(const QMetaMethod& signal)
{
    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::aboutToBeRemoved))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAboutToBeRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::removed))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChanged))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataSelectionChanged))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::childAdded))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetChildAdded));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::childRemoved))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetChildRemoved));
}

void DatasetPrivate::disconnectNotify(const QMetaMethod& signal)
{
    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::aboutToBeRemoved))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetAboutToBeRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::removed))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChanged))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataSelectionChanged))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetDataSelectionChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::childAdded))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetChildAdded));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::childRemoved))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DatasetChildRemoved));
}

void DatasetPrivate::registerDatasetEvents()
{
    try
    {
        _eventListener.registerDataEvent([this](DatasetEvent* dataEvent) {
            switch (dataEvent->getType()) {

                case EventType::DatasetAboutToBeRemoved:
                {
                    if (dataEvent->getDataset().getDatasetId() != getDatasetId())
                        break;

                    emit aboutToBeRemoved();

                    break;
                }

                case EventType::DatasetRemoved:
                {
                    const auto dataRemovedEvent = static_cast<DatasetRemovedEvent*>(dataEvent);

                    if (_datasetId != dataRemovedEvent->getDatasetGuid())
                        break;

                    reset();

                    emit removed(_datasetId);
                }

                case EventType::DatasetDataChanged:
                {
                    if (dataEvent->getDataset().getDatasetId() != getDatasetId())
                        break;

                    emit dataChanged();

                    break;
                }

                case EventType::DatasetDataSelectionChanged:
                {
                    if (dataEvent->getDataset().getDatasetId() != getDatasetId())
                        break;

                    emit dataSelectionChanged();

                    break;
                }

                case EventType::DatasetChildAdded:
                {
                    if (dataEvent->getDataset().getDatasetId() != getDatasetId())
                        break;

                    auto dataChildAddedEvent = static_cast<hdps::DatasetChildAddedEvent*>(dataEvent);

                    emit childAdded(dataChildAddedEvent->getChildDataset());

                    break;
                }

                case EventType::DatasetChildRemoved:
                {
                    if (dataEvent->getDataset().getDatasetId() != getDatasetId())
                        break;

                    auto dataChildRemovedEvent = static_cast<hdps::DatasetChildRemovedEvent*>(dataEvent);

                    emit childRemoved(dataChildRemovedEvent->getChildDatasetGuid());

                    break;
                }

                case EventType::DatasetAdded:
                case EventType::DatasetLocked:
                case EventType::DatasetUnlocked:
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
