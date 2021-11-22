#pragma once

#include "DatasetSignals.h"

#include "event/EventListener.h"
#include "util/Exception.h"

#include <QString>

namespace hdps
{

class DatasetImpl;
class CoreInterface;

/**
 * Smart dataset pointer private class
 *
 * Designed to emit signals related to changes in the dataset reference
 *
 * @author T. Kroes
 */
class DatasetPrivate : public EventListener
{
    /** Only dataset reference template classes have access to protected members */
    template<typename> friend class Dataset;

protected:

    /** Default constructor */
    DatasetPrivate() :
        EventListener(),
        _datasetId(),
        _dataset(nullptr),
        _signals()
    {
    }

    /**
     * Copy constructor
     * @param other Object to copy from
     */
    DatasetPrivate(const DatasetPrivate& other) :
        EventListener(),
        _datasetId(),
        _dataset(nullptr),
        _signals()
    {
    }

    /**
     * Assignment operator
     * @param other Object to assign from
     */
    DatasetPrivate& operator=(DatasetPrivate other) {
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

    /** Register to dataset events initialization */
    void registerDatasetEvents();

public:

    /** Get dataset GUID */
    virtual QString getDatasetGuid() const = 0;

    /** Get data signals */
    DatasetSignals& getSignals()
    {
        return _signals;
    }

protected:
    QString             _datasetId;     /** Globally unique dataset identifier */
    DatasetImpl*        _dataset;       /** Pointer to the dataset (if any) */
    DatasetSignals      _signals;       /** For emitting signals */
};

}
