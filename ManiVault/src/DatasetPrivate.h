// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>
#include <QObject>

namespace mv
{

class DatasetImpl;
class CoreInterface;

/** Only dataset reference template classes have access to protected members */
template<typename> class Dataset;

/**
 * @brief Private shared state for Dataset smart references.
 *
 * DatasetPrivate stores the dataset identifier and optional DatasetImpl pointer
 * shared by Dataset reference wrappers. It listens for core dataset events and
 * re-emits them as Qt signals so Dataset users can observe changes without
 * owning the dataset implementation.
 *
 * @maintainer T. Kroes
 */
class CORE_EXPORT DatasetPrivate : public QObject
{
    Q_OBJECT

    template<typename>
    friend class Dataset;

protected: // Construction/destruction

    /** Constructs an empty dataset private state object. */
    DatasetPrivate();

    DatasetPrivate(const DatasetPrivate& other) = delete;
    DatasetPrivate& operator=(const DatasetPrivate&) = delete;
	DatasetPrivate(DatasetPrivate&& other) = delete;
	DatasetPrivate& operator=(DatasetPrivate&& other) = delete;

    /** Destroys the dataset private state object. */
    ~DatasetPrivate() override = default;

public: // Member access and reset

    /** Returns the globally unique identifier of the dataset. */
    QString getDatasetId() const;

    /**
     * @brief Sets the globally unique identifier of the dataset.
     * @param datasetId Globally unique identifier of the dataset.
     */
    void setDatasetId(const QString& datasetId);

    /**
     * @brief Returns the dataset implementation pointer.
     * @return Dataset implementation pointer, or nullptr.
     */
    DatasetImpl* getDataset();

    /**
     * @brief Returns the dataset implementation pointer.
     * @return Dataset implementation pointer, or nullptr.
     */
    const DatasetImpl* getDataset() const;

    /**
     * @brief Sets the dataset implementation pointer.
     * @param dataset Dataset implementation pointer, or nullptr.
     */
    void setDataset(DatasetImpl* dataset);

    /**
     * @brief Resets the dataset pointer and disconnects from dataset events.
     * @param notify Whether to notify listeners about the reset.
     */
    void reset(bool notify = true);

public: // Keep track of when someone connects/disconnects from our signal(s)

    /**
     * @brief Registers event listening when a signal gains a connection.
     * @param signal Signal that received a connection.
     */
    void connectNotify(const QMetaMethod& signal) override;

    /**
     * @brief Updates event listening when a signal loses a connection.
     * @param signal Signal that lost a connection.
     */
    void disconnectNotify(const QMetaMethod& signal) override;

protected:

    /** Registers for dataset events from the core. */
    void registerDatasetEvents();

signals:

    /**
     * @brief Emitted when the dataset pointer changes.
     * @param dataset Current dataset pointer, or nullptr.
     */
    void changed(DatasetImpl* dataset);

    /** Emitted before the dataset is removed. */
    void aboutToBeRemoved();

    /**
     * @brief Emitted after the dataset has been removed.
     * @param datasetId Globally unique identifier of the removed dataset.
     */
    void removed(const QString& datasetId);

    /** Emitted when dataset data changes. */
    void dataChanged();

    /** Emitted when dataset dimensions change. */
    void dataDimensionsChanged();

    /** Emitted when dataset selection changes. */
    void dataSelectionChanged();

    /** Emitted when the dataset GUI name changes. */
    void guiNameChanged();

    /**
     * @brief Emitted when a child dataset is added.
     * @param childDataset Added child dataset.
     */
    void childAdded(const Dataset<DatasetImpl>& childDataset);

    /**
     * @brief Emitted when a child dataset is removed.
     * @param childDatasetGuid GUID of the removed child dataset.
     */
    void childRemoved(const QString& childDatasetGuid);

private:

    QString                         _datasetId;                     /**< Globally unique identifier of the dataset */
    DatasetImpl*                    _dataset;                       /**< Dataset implementation pointer, if available */
    std::unique_ptr<EventListener>  _eventListener;                 /**< Event listener for core dataset events */
    bool                            _eventsRegistered = false;      /**< Whether dataset events have been registered */
    QSet<std::uint32_t>             _pendingSupportedEventTypes;    /**< Event types to register once the dataset is available */

    friend class DatasetImpl;
};

/**
 * @brief Compares dataset private states for equality.
 * @param lhs Left-hand dataset private state.
 * @param rhs Right-hand dataset private state.
 * @return True if both states refer to the same dataset id.
 */
CORE_EXPORT inline bool operator == (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetId() == rhs.getDatasetId();
}

/**
 * @brief Compares dataset private states for inequality.
 * @param lhs Left-hand dataset private state.
 * @param rhs Right-hand dataset private state.
 * @return True if the states refer to different dataset ids.
 */
CORE_EXPORT inline bool operator != (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetId() != rhs.getDatasetId();
}

}
