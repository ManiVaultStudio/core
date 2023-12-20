// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EventManager.h"

#include <util/Exception.h>

#include <Set.h>
#include <LinkedData.h>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define EVENT_MANAGER_VERBOSE
#endif

namespace mv
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
        DatasetAddedEvent dataEvent(dataset);

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
        DatasetAboutToBeRemovedEvent dataAboutToBeRemovedEvent(dataset);

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
        DatasetRemovedEvent dataRemovedEvent(nullptr, datasetId, dataType);

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

void EventManager::notifyDatasetDataChanged(const Dataset<DatasetImpl>& dataset)
{
    try {
        DatasetDataChangedEvent dataEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that dataset data has changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that dataset data has changed");
    }
}

void EventManager::notifyDatasetDataDimensionsChanged(const Dataset<DatasetImpl>& dataset)
{
    try {
        DatasetDataDimensionsChangedEvent dataEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataEvent);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to notify that dataset data dimensions have changed", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to notify that dataset data dimensions have changed");
    }
}

void EventManager::notifyDatasetDataSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets /*= nullptr*/)
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

            notifyDatasetDataSelectionChanged(notifyDataset, ignoreDatasets);
        };

        DatasetDataSelectionChangedEvent dataSelectionChangedEvent(dataset);

        const auto eventListeners = _eventListeners;

        for (auto listener : eventListeners)
            if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                callListenerDataEvent(listener, &dataSelectionChangedEvent);

        if (!dataset->isFull())
            callNotifyDatasetSelectionChanged(dataset->getFullDataset<DatasetImpl>());

        if (dataset->isProxy())
            for (auto proxyMember : dataset->getProxyMembers())
                callNotifyDatasetSelectionChanged(proxyMember->getSourceDataset<DatasetImpl>());

        for (auto candidateDataset : mv::data().getAllDatasets()) {

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

void EventManager::notifyDatasetLocked(const Dataset<DatasetImpl>& dataset)
{
    try {

        if (!dataset.isValid())
            throw std::runtime_error("Dataset is invalid");

        DatasetLockedEvent dataLockedEvent(dataset);

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

        DatasetUnlockedEvent dataUnlockedEvent(dataset);

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

void EventManager::initialize()
{
#ifdef EVENT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractEventManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    endInitialization();
}

void EventManager::reset()
{
    _eventListeners.clear();
}

}
