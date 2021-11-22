#pragma once

#include "DatasetPrivate.h"

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>

namespace hdps
{

class DatasetImpl;
class CoreInterface;

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
class Dataset : public DatasetPrivate
{
public:

    /**
     * (Default) constructor
     * @param dataset Pointer to dataset (if any)
     */
    Dataset(DatasetType* dataset = nullptr) :
        DatasetPrivate()
    {
        set(dataset);
    }

    /**
     * Constructor
     * @param dataset Reference to dataset
     */
    Dataset(DatasetType& dataset) :
        DatasetPrivate()
    {
        set(&dataset);
    }

    /**
     * Copy constructor
     * @param other Smart pointer to copy from
     */
    Dataset(const Dataset<DatasetType>& other)
    {
        set(other.get());

        *this = other;
    }

    /**
     * Copy constructor
     * @param other Smart pointer to copy from
     */
    template<typename OtherDatasetType>
    Dataset(const Dataset<OtherDatasetType>& other)
    {
        set(other.get<DatasetType>());
    }

public: // Operators

    /** Dereference operator */
    DatasetType& operator* ()
    {
        return dynamic_cast<DatasetType&>(*_dataset);
    }

    /** Const dereference operator */
    const DatasetType& operator* () const
    {
        return dynamic_cast<DatasetType&>(*_dataset);
    }

public: // Pointer access

    /** Arrow operator */
    DatasetType* operator-> ()
    {
        return get<DatasetType>();
    }

    /** Arrow operator */
    const DatasetType* operator-> () const
    {
        return get<DatasetType>();
    }

    /** Parenthesis operator */
    DatasetType* operator() () const
    {
        return get<DatasetType>();
    }

    /** Get the dataset pointer */
    template<typename TargetSetType>
    TargetSetType* get() const
    {
        try
        {
            Q_ASSERT(_dataset != nullptr);

            // Cast to target type
            auto dataset = dynamic_cast<TargetSetType*>(_dataset);

            // Except if conversion fails
            if (_dataset == nullptr)
                throw std::runtime_error("Smart pointer is null, or the conversion to the target type failed");

            return dataset;
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to retrieve pointer from dataset smart pointer", e.what());
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to retrieve pointer from dataset smart pointer");
        }
        
        return nullptr;
    }

    /** Get the dataset pointer */
    DatasetImpl* get() const
    {
        return _dataset;
    }

    /**
     * Set the smart pointer
     * @param dataset Pointer to dataset
     */
    template<typename DatasetType>
    void set(DatasetType* dataset)
    {
        DatasetPrivate::set(static_cast<DatasetImpl*>(dataset));
    }

public: // Miscellaneous

    /** Get the smart pointer of the specified type */
    template<typename TargetSetType>
    Dataset<TargetSetType> getConverted() const
    {
        return Dataset<TargetSetType>(_dataset->get<TargetSetType>());
    }

    /** Returns whether the dataset pointer is valid (if the dataset actually exists) */
    bool isValid() const
    {
        return _dataset != nullptr;
    }

    /** Get the current dataset globally unique identifier */
    QString getDatasetGuid() const override
    {
        return _datasetId;
    }

    /** Resets the internals (dataset pointer to nullptr etc.) */
    void reset() override
    {
        _dataset    = nullptr;
        _datasetId  = "";
    }

public: // Operators

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    //const bool operator == (const Dataset<DatasetType>& rhs) const {
    //    return rhs.getDatasetGuid() == _datasetId;
    //}

    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    //const bool operator != (const Dataset<DatasetType>& rhs) const {
    //    return rhs.getDatasetGuid() != _datasetId;
    //}

    /**
     * Assignment operator
     * @param other Reference to assign from
     */
    Dataset<DatasetType>& Dataset<DatasetType>::operator=(const Dataset<DatasetType>& other)
    {
        set(other.get());

        return *this;
    }
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
