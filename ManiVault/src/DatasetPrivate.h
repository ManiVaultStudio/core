// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>

namespace mv
{

class DatasetImpl;
class CoreInterface;

/** Only dataset reference template classes have access to protected members */
template<typename> class Dataset;

/**
 * Smart dataset pointer private base class
 *
 * The primary aim of this class is to:
 * - Save the globally unique identifier of the dataset
 * - Save a pointer to a dataset (if initialized properly)
 * - Intercept events related to the dataset and rebroadcast them using Qt signals
 *
 * @author T. Kroes
 */
class CORE_EXPORT DatasetPrivate : public QObject
{
    Q_OBJECT

    template<typename>
    friend class Dataset;

protected: // Construction/destruction

    /** Default constructor */
    DatasetPrivate();

    DatasetPrivate(const DatasetPrivate& other) = delete;
    DatasetPrivate& operator=(const DatasetPrivate&) = delete;
	DatasetPrivate(DatasetPrivate&& other) = delete;
	DatasetPrivate& operator=(DatasetPrivate&& other) = delete;

    /** Remove the destructor */
    ~DatasetPrivate() override = default;

public: // Member access and reset

    /** Get the globally unique identifier of the dataset */
    QString getDatasetId() const;

    /** Set the globally unique identifier of the dataset
     * @param datasetId Globally unique identifier of the dataset
     */
    void setDatasetId(const QString& datasetId);

    /**
     * Get pointer to the dataset
     * @return Pointer to the dataset
     */
    DatasetImpl* getDataset();

    /**
     * Get pointer to the dataset
     * @return Const pointer to the dataset
     */
    const DatasetImpl* getDataset() const;

    /**
     * Set pointer to the dataset
     * @param dataset Pointer to the dataset
     */
    void setDataset(DatasetImpl* dataset);

    /** Resets the smart pointer */
    void reset();

public: // Keep track of when someone connects/disconnects from our signal(s)

    /**
     * This function is called when something has been connected to \p signal
     * @param signal The signal to which a connection has been made
     */
    void connectNotify(const QMetaMethod& signal) override;

    /**
     * This function is called when something has been disconnected from signal
     * @param signal The signal of which a connection has been broken
     */
    void disconnectNotify(const QMetaMethod& signal) override;

protected:

    /** Register for data events from the core */
    void registerDatasetEvents();

signals:

    /**
     * Signals that the pointer to the dataset changed
     * @param dataset Pointer to current dataset
     */
    void changed(DatasetImpl* dataset);

    /** Signals that the dataset is about to be removed */
    void aboutToBeRemoved();

    /**
     * Signals that the dataset has been removed
     * @param datasetId Globally unique identifier of the dataset that is removed
     */
    void removed(const QString& datasetId);

    /** Signals that the dataset data changed */
    void dataChanged();

    /** Signals that the dataset dimensions changed */
    void dataDimensionsChanged();

    /** Signals that the dataset selection changed */
    void dataSelectionChanged();

    /** Signals that the dataset GUI name changed */
    void guiNameChanged();

    /**
     * Signals that a dataset child is added
     * @param childDataset Smart pointer to added child dataset
     */
    void childAdded(const Dataset<DatasetImpl>& childDataset);

    /**
     * Signals that a dataset child was removed
     * @param childDatasetGuid GUID of the dataset child that is removed
     */
    void childRemoved(const QString& childDatasetGuid);

private:
    QString                         _datasetId;                     /** Globally unique identifier of the dataset */
    DatasetImpl*                    _dataset;                       /** Pointer to the dataset (if any) */
    std::unique_ptr<EventListener>  _eventListener;                 /** Listen to ManiVault events */
    bool                            _eventsRegistered = false;      /** Whether events have been registered */
    QSet<std::uint32_t>             _pendingSupportedEventTypes;    /** Pending supported event types (to be registered when the dataset is set) */

    friend class DatasetImpl;
};

/**
 * Compares two dataset smart pointers for equality
 * @param lhs Left hand side dataset smart pointer
 * @param rhs Right hand side dataset smart pointer
 * @return Whether lhs and rhs are equal
 */
CORE_EXPORT inline bool operator == (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetId() == rhs.getDatasetId();
}

/**
 * Compares two dataset smart pointers for inequality
 * @param lhs Left hand side dataset smart pointer
 * @param rhs Right hand side dataset smart pointer
 * @return Whether lhs and rhs are not equal
 */
CORE_EXPORT inline bool operator != (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetId() != rhs.getDatasetId();
}

}
