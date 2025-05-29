// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "EventManager.h"

#include <util/Exception.h>

#include <event/Event.h>

#include <Set.h>
#include <LinkedData.h>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define EVENT_MANAGER_VERBOSE
#endif

namespace mv
{
constexpr int EVENT_POLLING_INTERVAL_MS = 20;

EventManager::EventManager(QObject* parent) :
    AbstractEventManager(parent)
{

}

EventManager::~EventManager()
{
    reset();
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

    _selectionPollingTimer = new QTimer(this);
    connect(_selectionPollingTimer, &QTimer::timeout, this, [this]() {
        Datasets datasets = data().getAllDatasets();

        // Make a temporary copy of the event listener list to avoid additions while processing
        const auto eventListeners = _eventListeners;

        // Propagate selection flags
        for (auto dataset : datasets)
        {
            if (!dataset->needsSelectionUpdate())
                continue;

            // For all datasets, check if they relate to the current dataset and notify them as well
            for (auto candidateDataset : mv::data().getAllDatasets()) {
                // Ignore the current dataset
                if (candidateDataset == dataset)
                    continue;

                // If the dataset derives from the current dataset, notify it
                if (candidateDataset->isDerivedData() && candidateDataset->getSourceDataset<DatasetImpl>()->getRawDataName() == dataset->getSourceDataset<DatasetImpl>()->getRawDataName())
                    candidateDataset->markSelectionDirty(true);

                // If the dataset has the same raw data, notify it
                if (candidateDataset->getRawDataName() == dataset->getRawDataName())
                    candidateDataset->markSelectionDirty(true);

                // If the dataset is a proxy and the current dataset is one of its members, notify the proxy
                if (candidateDataset->isProxy() && candidateDataset->getProxyMembers().contains(dataset))
                    candidateDataset->markSelectionDirty(true);
            }
        }

        // For all dirty dataset selections, fire an event
        for (auto dataset : datasets)
        {
            if (!dataset->needsSelectionUpdate())
                continue;
            
            DatasetDataSelectionChangedEvent dataSelectionChangedEvent(dataset);
            
            // For every listener, find it in the original list and call its DataSelectionChangedEvent
            for (auto listener : eventListeners)
                if (std::find(_eventListeners.begin(), _eventListeners.end(), listener) != _eventListeners.end())
                    callListenerDataEvent(listener, &dataSelectionChangedEvent);

            dataset->markSelectionDirty(false);
        }
        });
    _selectionPollingTimer->start(EVENT_POLLING_INTERVAL_MS);
}

void EventManager::reset()
{
#ifdef DATA_HIERARCHY_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        _eventListeners.clear();
    }
    endReset();
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
#ifdef EVENT_MANAGER_VERBOSE
        QStringList datasetNotifiedString;

        if (ignoreDatasets != nullptr)
            for (auto datasetNotified : *ignoreDatasets)
                datasetNotifiedString << datasetNotified->getGuiName();

        qDebug() << __FUNCTION__ << dataset->getGuiName() << datasetNotifiedString;
#endif

        if (ignoreDatasets != nullptr && ignoreDatasets->contains(dataset))
            return;

        Datasets notified{ dataset };

        if (ignoreDatasets == nullptr)
            ignoreDatasets = &notified;
        else
            *ignoreDatasets << dataset;

        dataset->markSelectionDirty(true);

        // For all selection groups, set the current dataset as having a changed selection
        for (const KeyBasedSelectionGroup& selectionGroup : _selectionGroups)
        {
            selectionGroup.selectionChanged(dataset, dataset->getSelection()->getSelectionIndices());
        }

        // If the dataset has any linked dataset, then dirty them as well
        for (const LinkedData& ld : dataset->getLinkedData()) {
            ld.getTargetDataset()->markSelectionDirty(true);
            notifyDatasetDataSelectionChanged(ld.getTargetDataset(), ignoreDatasets);
        }
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

void EventManager::fromVariantMap(const QVariantMap& variantMap)
{
    AbstractEventManager::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "SelectionGroups");

    QVariantList selectionGroupsList = variantMap["SelectionGroups"].value<QVariantList>();

    _selectionGroups.resize(selectionGroupsList.size());
    for (int i = 0; i < selectionGroupsList.size(); i++)
        _selectionGroups[i].fromVariantMap(selectionGroupsList[i].toMap());
}

QVariantMap EventManager::toVariantMap() const
{
    QVariantMap variantMap = AbstractEventManager::toVariantMap();

    QVariantList selectionGroups;

    for (auto& selectionGroup : _selectionGroups)
    {
        selectionGroups.append(selectionGroup.toVariantMap());
    }

    variantMap["SelectionGroups"] = selectionGroups;

    return variantMap;
}

}
