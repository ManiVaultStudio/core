// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Dataset.h"

namespace mv
{

class DatasetImpl;
class KeyBasedSelectionGroup;

/**
 * Abstract event manager
 *
 * Base abstract event manager class for managing events.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractEventManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractEventManager(QObject* parent) :
        AbstractManager(parent, "Events")
    {
    }

    virtual ~AbstractEventManager() { }

    virtual std::vector<KeyBasedSelectionGroup>& getSelectionGroups() = 0;

    virtual void addSelectionGroup(KeyBasedSelectionGroup& selectionGroup) = 0;

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    virtual void notifyDatasetAdded(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    virtual void notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    virtual void notifyDatasetRemoved(const QString& datasetGuid, const DataType& dataType) = 0;

    /**
     * Notify listeners that a dataset has changed data
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    virtual void notifyDatasetDataChanged(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that a dataset has changed data dimensions
     * @param dataset Smart pointer to the dataset of which the data dimensions changed
     */
    virtual void notifyDatasetDataDimensionsChanged(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify listeners that dataset data selection has changed
     * @param dataset Smart pointer to the dataset of which the data selection changed
     * @param ignoreDatasets Pointer to datasets that should be ignored during notification
     */
    virtual void notifyDatasetDataSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets = nullptr) = 0;

    /**
     * Notify all listeners that a dataset is locked
     * @param dataset Smart pointer to the dataset
     */
    virtual void notifyDatasetLocked(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Notify all listeners that a dataset is unlocked
     * @param dataset Smart pointer to the dataset
     */
    virtual void notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset) = 0;

    /**
     * Register an event listener
     * @param eventListener Pointer to event listener to register
     */
    virtual void registerEventListener(EventListener* eventListener) = 0;

    /**
     * Unregister an event listener
     * @param eventListener Pointer to event listener to unregister
     */
    virtual void unregisterEventListener(EventListener* eventListener) = 0;

protected:

    /**
     * Calls \p dataEvent of the \p eventListener
     * @param eventListener Pointer to event listener
     * @param dataEvent Pointer to data event
     */
    void callListenerDataEvent(EventListener* eventListener, DatasetEvent* dataEvent) {
        Q_ASSERT(eventListener != nullptr);
        Q_ASSERT(dataEvent != nullptr);

        eventListener->onDataEvent(dataEvent);
    }
};

}
