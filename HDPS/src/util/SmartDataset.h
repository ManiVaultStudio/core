#pragma once

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>

namespace hdps
{

class DatasetImpl;
class CoreInterface;

namespace util
{

/**
 * Dataset reference private class
 *
 * Designed to emit signals related to changes in the dataset reference
 *
 * @author T. Kroes
 */
class SmartDatasetPrivate : public QObject, public EventListener
{
    Q_OBJECT

    /** Only dataset reference template classes have access to protected members */
    template<typename> friend class SmartDataset;

protected:

    /**
     * Default constructor
     * @param parent Pointer to parent object
     */
    SmartDatasetPrivate() :
        QObject(),
        _datasetId(),
        _dataset(nullptr)
    {
    }

    /**
     * Copy constructor
     * @param other Object to copy from
     */
    SmartDatasetPrivate(const SmartDatasetPrivate& other) :
        QObject(),
        _datasetId(),
        _dataset(nullptr)
    {
    }

    /**
     * Assignment operator
     * @param other Object to assign from
     */
    SmartDatasetPrivate& operator=(SmartDatasetPrivate other) {
        return *this;
    }

    /** Reset the smart pointer */
    virtual void reset() = 0;

    /**
     * Set the smart pointer
     * @param dataset Pointer to dataset
     */
    void set(DatasetImpl* dataset);

protected:

    /** Perform initialization */
    void init();

public:

    /** Get dataset GUID */
    virtual QString getDatasetGuid() const = 0;

signals:

    /**
     * Signals that the pointer to the dataset changed
     * @param dataset Pointer to current dataset
     */
    void changed(DatasetImpl* dataset);

    /** Signals that the dataset contents changed */
    void dataChanged();

    /** Signals that the dataset is about to be removed */
    void dataAboutToBeRemoved();

    /**
     * Signals that the dataset has been removed
     * @param datasetId Globally unique identifier of the dataset that is removed
     */
    void dataRemoved(const QString& datasetId);

    /**
     * Signals that the dataset GUI name changed
     * @param oldGuiName Old GUI name
     * @param newGuiName New GUI name
     */
    void dataGuiNameChanged(const QString& oldGuiName, const QString& newGuiName);

protected:
    QString         _datasetId;     /** Globally unique dataset identifier */
    DatasetImpl*    _dataset;       /** Pointer to the dataset (if any) */
};

/**
 * Smart dataset class
 *
 * Smart dataset pointer for datasets
 * 
 * This smart pointer ensures that access to datasets through pointers 
 * is more secure and requires less code. It internally manages a pointer
 * throughout the lifetime of a dataset (resets the datasets to nullptr
 * when a dataset is removed etc.).
 *
 * @author T. Kroes
 */
template<typename DatasetType>
class SmartDataset : public SmartDatasetPrivate
{
public:

    /**
     * (Default) constructor
     * @param dataset Pointer to dataset (if any)
     */
    SmartDataset(DatasetType* dataset = nullptr) :
        SmartDatasetPrivate()
    {
        // Perform startup initialization
        init();

        // Assign dataset
        set(*dataset);
    }

    /**
     * Constructor
     * @param dataset Reference to dataset
     */
    SmartDataset(DatasetType& dataset) :
        SmartDatasetPrivate()
    {
        // Perform startup initialization
        init();

        // Assign dataset
        set(dataset);
    }

    /**
     * Copy constructor
     * @param other Smart dataset reference to copy from
     */
    SmartDataset(const SmartDataset& other)
    {
        // Initialize
        init();

        // And assign
        *this = other;
    }

public:

    /**
     * Set the pointer to dataset
     * @param dataset Reference to the dataset
     */
    void set(DatasetType& dataset) {
        SmartDatasetPrivate::set(&dataset);
    }

    /** Dereference operator */
    DatasetType& operator* () {
        return dynamic_cast<DatasetType&>(*_dataset);
    }

    /** Const dereference operator */
    const DatasetType& operator* () const {
        return dynamic_cast<DatasetType&>(*_dataset);
    }

    /** Arrow operator */
    DatasetType* operator-> () {
        return get();
    }

    /** Arrow operator */
    const DatasetType* operator-> () const {
        return get();
    }

    /** Parenthesis operator */
    DatasetType* operator() () const {
        return dynamic_cast<DatasetType*>(_dataset);
    }

    /** Get the dataset pointer */
    DatasetType* get() const {
        return dynamic_cast<DatasetType*>(_dataset);
    }

    /** Get the dataset pointer of the specified type */
    template<typename TargetSetType>
    TargetSetType* get() const {
        return dynamic_cast<TargetSetType*>(_dataset);
    }

    /** Get the smart pointer of the specified type */
    template<typename TargetSetType>
    SmartDataset<TargetSetType> converted() const {
        return SmartDataset<TargetSetType>(dynamic_cast<TargetSetType*>(_dataset));
    }

    /** Returns whether the dataset pointer is valid (if the dataset actually exists) */
    bool isValid() const {
        return _dataset != nullptr;
    }

    /** Get the current dataset globally unique identifier */
    QString getDatasetGuid() const override {
        return _datasetId;
    }

    /** Resets the internals (dataset pointer to nullptr etc.) */
    void reset() override {
        _dataset    = nullptr;
        _datasetId  = "";
    }

public: // Operators

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator == (const SmartDataset<DatasetType>& rhs) const {
        return rhs.getDatasetGuid() == _datasetId;
    }

    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    const bool operator != (const SmartDataset<DatasetType>& rhs) const {
        return rhs.getDatasetGuid() != _datasetId;
    }

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    SmartDataset<DatasetType>& SmartDataset<DatasetType>::operator=(const SmartDataset<DatasetType>& other)
    {
        SmartDatasetPrivate::operator=(other);

        _datasetId  = other._datasetId;
        _dataset    = other._dataset;

        return *this;
    }
};

/**
 * Compares two smart pointers for equality
 * @param lhs Left hand side smart pointer
 * @param rhs Right hand side smart pointer
 * @return Whether lhs and rhs are equal
 */
inline bool operator == (const SmartDatasetPrivate& lhs, const SmartDatasetPrivate& rhs) {
    return lhs.getDatasetGuid() == rhs.getDatasetGuid();
}

/**
 * Compares two smart pointers for inequality
 * @param lhs Left hand side smart pointer
 * @param rhs Right hand side smart pointer
 * @return Whether lhs and rhs are not equal
 */
inline bool operator != (const SmartDatasetPrivate& lhs, const SmartDatasetPrivate& rhs) {
    return lhs.getDatasetGuid() != rhs.getDatasetGuid();
}

}

// Use an alias from now on for brevity and clarity
template<typename DatasetType>
using Dataset = util::SmartDataset<DatasetType>;



}
