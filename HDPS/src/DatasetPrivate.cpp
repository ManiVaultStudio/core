#include "DatasetPrivate.h"

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
    _datasetGuid(),
    _dataset(nullptr),
    _eventListener()
{
    registerDatasetEvents();
}

DatasetPrivate::DatasetPrivate(const DatasetPrivate& other) :
    _datasetGuid(),
    _dataset(nullptr),
    _eventListener()
{
    registerDatasetEvents();
}

QString DatasetPrivate::getDatasetGuid() const
{
    return _datasetGuid;
}

void DatasetPrivate::setDatasetGuid(const QString& datasetGuid)
{
    if (datasetGuid == _datasetGuid)
        return;

    _datasetGuid = datasetGuid;

    if (_datasetGuid.isEmpty()) {
        reset();
    }
    else {
        _dataset = getDataset();

        emit changed(_dataset);
    }
}

DatasetImpl* DatasetPrivate::getDataset()
{
    //if (_dataset == nullptr && !_datasetGuid.isEmpty())
    //    _dataset = Application::core()->requestDataset(_datasetGuid).get();

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
        _datasetGuid    = _dataset->getGuid();

        emit changed(_dataset);
    }
}

void DatasetPrivate::reset()
{
    _dataset        = nullptr;
    _datasetGuid    = "";
}

void DatasetPrivate::connectNotify(const QMetaMethod& signal)
{
    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataAboutToBeRemoved))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataAboutToBeRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataRemoved))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChanged))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataSelectionChanged))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataGuiNameChanged))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataGuiNameChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChildAdded))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChildAdded));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChildRemoved))
        _eventListener.addSupportedEventType(static_cast<std::uint32_t>(EventType::DataChildRemoved));
}

void DatasetPrivate::disconnectNotify(const QMetaMethod& signal)
{
    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataAboutToBeRemoved))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataAboutToBeRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataRemoved))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataRemoved));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChanged))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataSelectionChanged))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataSelectionChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataGuiNameChanged))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataGuiNameChanged));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChildAdded))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataChildAdded));

    if (signal == QMetaMethod::fromSignal(&DatasetPrivate::dataChildRemoved))
        _eventListener.removeSupportedEventType(static_cast<std::uint32_t>(EventType::DataChildRemoved));
}

void DatasetPrivate::registerDatasetEvents()
{
    try
    {
        _eventListener.setEventCore(Application::core());

        _eventListener.registerDataEvent([this](DataEvent* dataEvent) {
            switch (dataEvent->getType()) {

                case EventType::DataAboutToBeRemoved:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    emit dataAboutToBeRemoved();

                    break;
                }

                case EventType::DataRemoved:
                {
                    const auto dataRemovedEvent = static_cast<DataRemovedEvent*>(dataEvent);

                    if (_datasetGuid != dataRemovedEvent->getDatasetGuid())
                        break;

                    reset();

                    emit dataRemoved(_datasetGuid);
                }

                case EventType::DataChanged:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    emit dataChanged();

                    break;
                }

                case EventType::DataSelectionChanged:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    emit dataSelectionChanged();

                    break;
                }

                case EventType::DataGuiNameChanged:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    auto datasetGuiNameChangedEvent = static_cast<hdps::DataGuiNameChangedEvent*>(dataEvent);

                    emit dataGuiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset()->getGuiName());

                    break;
                }

                case EventType::DataChildAdded:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    auto dataChildAddedEvent = static_cast<hdps::DataChildAddedEvent*>(dataEvent);

                    emit dataChildAdded(dataChildAddedEvent->getChildDataset());

                    break;
                }

                case EventType::DataChildRemoved:
                {
                    if (dataEvent->getDataset().getDatasetGuid() != getDatasetGuid())
                        break;

                    auto dataChildRemovedEvent = static_cast<hdps::DataChildRemovedEvent*>(dataEvent);

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
