// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractEventManager.h"

#include <event/EventListener.h>

#include "SelectionGroup.h"

#include <QTimer>

namespace mv
{
    
class EventManager final : public AbstractEventManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    EventManager(QObject* parent);

    /** Reset when destructed */
    ~EventManager() override;

    /** Perform event manager startup initialization */
    void initialize() override;

    /** Resets the contents of the event manager */
    void reset() override;

    void addSelectionGroup(KeyBasedSelectionGroup& selectionGroup) override { _selectionGroups.push_back(std::move(selectionGroup)); }

    /**
     * Notify listeners that a new dataset has been added to the core
     * @param dataset Smart pointer to the dataset that was added
     */
    void notifyDatasetAdded(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is about to be removed
     * @param dataset Smart pointer to the dataset which is about to be removed
     */
    void notifyDatasetAboutToBeRemoved(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset is removed
     * @param datasetGuid GUID of the dataset that was removed
     * @param dataType Type of the data
     */
    void notifyDatasetRemoved(const QString& datasetGuid, const DataType& dataType) override;

    /**
     * Notify listeners that a dataset has changed data
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    void notifyDatasetDataChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that a dataset has changed data dimensions
     * @param dataset Smart pointer to the dataset of which the data dimensions changed
     */
    void notifyDatasetDataDimensionsChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that dataset data selection has changed
     * @param dataset Smart pointer to the dataset of which the data selection changed
     * @param ignoreDatasets Pointer to datasets that should be ignored during notification
     */
    void notifyDatasetDataSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets = nullptr) override;

    /**
     * Notify all listeners that a dataset is locked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetLocked(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify all listeners that a dataset is unlocked
     * @param dataset Smart pointer to the dataset
     */
    void notifyDatasetUnlocked(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Register an event listener
     * @param eventListener Pointer to event listener to register
     */
    void registerEventListener(EventListener* eventListener) override;

    /**
     * Unregister an event listener
     * @param eventListener Pointer to event listener to unregister
     */
    void unregisterEventListener(EventListener* eventListener) override;

public: // Serialization

    /**
     * Load event manager from variant
     * @param Variant representation of the event manager
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save event manager to variant
     * @return Variant representation of the event manager
     */
    QVariantMap toVariantMap() const override;

private:
    std::vector<EventListener*>         _eventListeners;    /** List of classes listening for core events */

    std::vector<KeyBasedSelectionGroup> _selectionGroups;   /** List of key-based selection groups used to synchronize selections between datasets */

    QTimer* _selectionPollingTimer;
};

}
