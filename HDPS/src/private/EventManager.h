#pragma once

#include "AbstractEventManager.h"

#include <event/EventListener.h>

namespace hdps
{

class EventManager final : public AbstractEventManager
{
    Q_OBJECT

public:

    /** Default constructor */
    EventManager();

    /** Perform event manager startup initialization */
    void initalize() override;

    /** Resets the contents of the event manager */
    void reset() override;

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
     * Notify listeners that a dataset has changed
     * @param dataset Smart pointer to the dataset of which the data changed
     */
    void notifyDatasetChanged(const Dataset<DatasetImpl>& dataset) override;

    /**
     * Notify listeners that dataset selection has changed
     * @param dataset Smart pointer to the dataset of which the selection changed
     * @param ignoreDatasets Pointer to datasets that should be ignored during notification
     */
    void notifyDatasetSelectionChanged(const Dataset<DatasetImpl>& dataset, Datasets* ignoreDatasets = nullptr) override;

    /**
     * Notify all listeners that a dataset GUI name has changed
     * @param dataset Smart pointer to the dataset of which the GUI name changed
     * @param previousGuiName Previous dataset name
     */
    void notifyDatasetGuiNameChanged(const Dataset<DatasetImpl>& dataset, const QString& previousGuiName) override;

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

private:
    std::vector<EventListener*>     _eventListeners;        /** List of classes listening for core events */
};

}