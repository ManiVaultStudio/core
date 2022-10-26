#pragma once

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>

namespace hdps
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
class DatasetPrivate : public QObject
{
    Q_OBJECT

    template<typename>
    friend class Dataset;

protected: // Construction/destruction

    /** Default constructor */
    DatasetPrivate();

    /**
     * Copy constructor
     * @param other Object to copy from
     */
    DatasetPrivate(const DatasetPrivate& other);

    /**
     * Assignment operator
     * @param other Object to assign from
     */
    DatasetPrivate& operator=(const DatasetPrivate& other) = delete;

    /** Remove the destructor */
    ~DatasetPrivate() override = default;

public: // Member access and reset

    /** Get the globally unique identifier of the dataset */
    QString getDatasetGuid() const;

    /** Set the globally unique identifier of the dataset
     * @param datasetGuid Globally unique identifier of the dataset
     */
    void setDatasetGuid(const QString& datasetGuid);

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
    void dataAboutToBeRemoved();

    /**
     * Signals that the dataset has been removed
     * @param datasetId Globally unique identifier of the dataset that is removed
     */
    void dataRemoved(const QString& datasetId);

    /** Signals that the dataset contents changed */
    void dataChanged();

    /** Signals that the dataset selection changed */
    void dataSelectionChanged();

    /**
     * Signals that the dataset GUI name changed
     * @param oldGuiName Old GUI name
     * @param newGuiName New GUI name
     */
    void dataGuiNameChanged(const QString& oldGuiName, const QString& newGuiName);

    /**
     * Signals that a dataset child is added
     * @param childDataset Smart pointer to added child dataset
     */
    void dataChildAdded(const Dataset<DatasetImpl>& childDataset);

    /**
     * Signals that a dataset child was removed
     * @param childDatasetGuid GUID of the dataset child that is removed
     */
    void dataChildRemoved(const QString& childDatasetGuid);

private:
    QString         _datasetGuid;       /** Globally unique identifier of the dataset */
    DatasetImpl*    _dataset;           /** Pointer to the dataset (if any) */
    EventListener   _eventListener;     /** Listen to HDPS events */
};

/**
 * Compares two dataset smart pointers for equality
 * @param lhs Left hand side dataset smart pointer
 * @param rhs Right hand side dataset smart pointer
 * @return Whether lhs and rhs are equal
 */
inline bool operator == (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetGuid() == rhs.getDatasetGuid();
}

/**
 * Compares two dataset smart pointers for inequality
 * @param lhs Left hand side dataset smart pointer
 * @param rhs Right hand side dataset smart pointer
 * @return Whether lhs and rhs are not equal
 */
inline bool operator != (const DatasetPrivate& lhs, const DatasetPrivate& rhs)
{
    return lhs.getDatasetGuid() != rhs.getDatasetGuid();
}

}
