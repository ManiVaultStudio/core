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
 * Smart dataset pointer private class
 *
 * Private internals for the dataset smart pointer
 *
 * @author T. Kroes
 */
class DatasetPrivate : public QObject, public EventListener
{
    Q_OBJECT

    template<typename>
    friend class Dataset;

protected:

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

public:

    /** Reset the smart pointer */
    virtual void reset() = 0;

    /**
     * Set the smart pointer
     * @param dataset Pointer to dataset
     */
    void set(DatasetImpl* dataset);

protected:

    /** Register to dataset events initialization */
    void registerDatasetEvents();

public:

    /** Get dataset GUID */
    virtual QString getDatasetGuid() const = 0;

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

protected:
    QString         _datasetGuid;       /** Globally unique dataset identifier */
    DatasetImpl*    _dataset;           /** Pointer to the dataset (if any) */
};

}
