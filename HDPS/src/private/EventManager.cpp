#include "EventManager.h"

#include <Application.h>

#include <util/Exception.h>

#include <Set.h>
#include <LinkedData.h>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define EVENT_MANAGER_VERBOSE
#endif

namespace hdps
{

EventManager::EventManager() :
    AbstractEventManager()
{
}

void EventManager::registerEventListener(EventListener* eventListener)
{
    _eventListeners.push_back(eventListener);
}

void EventManager::unregisterEventListener(EventListener* eventListener)
{
    _eventListeners.erase(std::remove(_eventListeners.begin(), _eventListeners.end(), eventListener), _eventListeners.end());
}

void EventManager::notifyDatasetAdded(const Dataset<DatasetImpl>& dataset)
{
    try {
        DataAddedEvent dataEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data was added", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data was added");
    }
}

void EventManager::notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset)
{
    try {
        DataAboutToBeRemovedEvent dataAboutToBeRemovedEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataAboutToBeRemovedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is about to be removed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is about to be removed");
    }
}

void EventManager::notifyDatasetRemoved(const QString& datasetId, const DataType& dataType)
{
    try {
        DataRemovedEvent dataRemovedEvent(nullptr, datasetId, dataType);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataRemovedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is removed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is removed");
    }
}

void EventManager::notifyDatasetChanged(const Dataset<DatasetImpl>& dataset)
{
    try {
        DataChangedEvent dataEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data is changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data is changed");
    }
}

void EventManager::notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets /*= nullptr*/)
{
    try {
        if (ignoreDatasets != nullptr && ignoreDatasets->contains(dataset))
            return;

        if (ignoreDatasets != nullptr)
            *ignoreDatasets << dataset;

        QStringList datasetNotifiedString;

#ifdef EVENT_MANAGER_VERBOSE
        if (ignoreDatasets != nullptr)
            for (auto datasetNotified : *ignoreDatasets)
                datasetNotifiedString << datasetNotified->getGuiName();

        qDebug() << __FUNCTION__ << dataset->getGuiName() << datasetNotifiedString;
#endif

        Datasets notified{ dataset };

        if (ignoreDatasets == nullptr)
            ignoreDatasets = &notified;

        const auto callNotifyDatasetSelectionChanged = [this, dataset, ignoreDatasets](Dataset<DatasetImpl> notifyDataset) -> void {
            if (ignoreDatasets != nullptr && ignoreDatasets->contains(notifyDataset))
                return;

            notifyDatasetSelectionChanged(notifyDataset, ignoreDatasets);
        };

        DataSelectionChangedEvent dataSelectionChangedEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataSelectionChangedEvent);

        if (!dataset->isFull())
            callNotifyDatasetSelectionChanged(dataset->getFullDataset<DatasetImpl>());

        if (dataset->isProxy())
            for (auto proxyMember : dataset->getProxyMembers())
                callNotifyDatasetSelectionChanged(proxyMember->getSourceDataset<DatasetImpl>());

        for (auto candidateDataset : core()->requestAllDataSets()) {

            if (ignoreDatasets != nullptr && ignoreDatasets->contains(candidateDataset))
                continue;

            if (candidateDataset == dataset)
                continue;

            if (candidateDataset->isDerivedData() && candidateDataset->getSourceDataset<DatasetImpl>()->getRawDataName() == dataset->getSourceDataset<DatasetImpl>()->getRawDataName())
                callNotifyDatasetSelectionChanged(candidateDataset);

            if (candidateDataset->getRawDataName() == dataset->getRawDataName())
                callNotifyDatasetSelectionChanged(candidateDataset);

            if (candidateDataset->isProxy() && candidateDataset->getProxyMembers().contains(dataset))
                callNotifyDatasetSelectionChanged(candidateDataset);
        }

        for (const LinkedData& ld : dataset->getLinkedData())
            callNotifyDatasetSelectionChanged(ld.getTargetDataset());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data selection has changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data selection has changed");
    }
}

void EventManager::notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName)
{
    try {

        DataGuiNameChangedEvent dataGuiNameChangedEvent(dataset, previousGuiName);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataGuiNameChangedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that data GUI name has changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that data GUI name has changed");
    }
}

void EventManager::notifyDatasetLocked(const Dataset<DatasetImpl>& dataset)
{
    try {

        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        DataLockedEvent dataLockedEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataLockedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data was locked", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data was locked");
    }
}

void EventManager::notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset)
{
    try {

        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        DataUnlockedEvent dataUnlockedEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataUnlockedEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that a data was unlocked", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that a data was unlocked");
    }
}

void EventManager::initalize()
{
}

void EventManager::reset()
{
    _eventListeners.clear();
}

}
