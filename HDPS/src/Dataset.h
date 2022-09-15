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
 * Smart pointer for datasets
 * 
 * This smart pointer aims to make dataset access through pointers more secure. It internally manages a pointer throughout the lifetime of a dataset (resets the datasets to nullptr when a dataset is removed etc.).
 * Access is through standard arrow and dereference operators. It is advised to always check for pointer validity before use of the pointer.
 * In addition, it emits data signals when data is renamed, removed etc.
 *
 * @author T. Kroes
 */
template<typename DatasetType>
class Dataset : public DatasetPrivate
{
public: // Construction

    /**
     * (Default) constructor
     * @param dataset Pointer to dataset (if any)
     */
    Dataset(DatasetType* dataset = nullptr) :
        DatasetPrivate()
    {
        setDataset(dataset);
    }

    /**
     * Construct from dataset reference
     * @param dataset Reference to dataset
     */
    Dataset(DatasetType& dataset) :
        DatasetPrivate()
    {
        setDataset(&dataset);
    }

    /**
     * Copy constructor
     * @param other Smart pointer to copy from
     */
    Dataset(const Dataset<DatasetType>& other)
    {
        setDataset(other.get());

        *this = other;
    }

    /**
     * Templated copy constructor
     * @param other Smart pointer to copy from
     */
    template<typename OtherDatasetType>
    Dataset(const Dataset<OtherDatasetType>& other)
    {
        setDataset(other.template get<DatasetType>());
    }

public: // Operators

    /** Dereference operator
     * @return Reference to dataset 
     */
    DatasetType& operator* ()
    {
        return dynamic_cast<DatasetType&>(*getDataset());
    }

    /** Const dereference operator
     * @return Reference to dataset
     */
    const DatasetType& operator* () const
    {
        return dynamic_cast<DatasetType&>(*getDataset());
    }

    /**
     * Assignment operator
     * @param rhs Right hand side operator
     */
    Dataset<DatasetType>& operator=(const Dataset<DatasetType>& rhs)
    {
        setDataset(rhs.get());

        return *this;
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

    /** Get the dataset pointer in the target dataset type */
    template<typename TargetSetType>
    TargetSetType* get() const
    {
        return reinterpret_cast<TargetSetType*>(const_cast<DatasetImpl*>(getDataset()));
    }

    /** Get the dataset implementation pointer */
    DatasetType* () const
    {
        return get<DatasetType>();
    }

    /** Returns smart pointer validity
     * @return Boolean determining whether the dataset pointer is valid (if the dataset actually exists)
     */
    bool isValid() const
    {
        return getDataset() != nullgetptr;
    }
};

using Datasets = QVector<Dataset<DatasetImpl>>;

}
