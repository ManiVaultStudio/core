#pragma once

#include "event/EventListener.h"

#include "Set.h"
#include "Application.h"

#include "util/Exception.h"

#include <QString>
#include <QMessageBox>

namespace hdps
{

class DataSet;
class CoreInterface;

namespace util
{

/**
 * Dataset reference class
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
class DatasetRef : EventListener
{
public:

    /**
     * Constructor
     * @param dataset Pointer to dataset (if any)
     */
    DatasetRef(DatasetType* dataset = nullptr) :
        EventListener(),
        _datasetId(),
        _dataset(nullptr)
    {
        // Perform startup initialization
        init();

        // Assign dataset
        set(dataset);
    }

    /**
     * Constructor
     * @param datasetId Globally unique identifier of the dataset
     */
    DatasetRef(const QString& datasetId) :
        _datasetId(datasetId),
        _dataset(nullptr)
    {
    }

    /** Initializes the dataset reference */
    void init() {

        // Set event core in the event listener
        setEventCore(Application::core());

        // Register for data removal events
        registerDataEvent([this](hdps::DataEvent* dataEvent) {

            // Only proceed if we have a valid data set
            if (_dataset == nullptr)
                return;

            switch (dataEvent->getType()) {

                // Reset the reference when the data is removed
                case EventType::DataRemoved:
                {
                    // Do not process foreign datasets
                    if (dataEvent->getDataset() != (*_dataset))
                        return;

                    // Reset the reference
                    reset();

                    break;
                }
            }
        });
    }

    /**
     * Set the reference from a pointer to a dataset
     * @param dataset Pointer to the dataset
     */
    void set(DataSet* dataset) {

        // Assign dataset pointer and cache the globally unique dataset identifier
        try
        {
            if (dataset == nullptr) {
                reset();
                return;
            }

            // No need to process the same dataset
            if (dataset == _dataset)
                return;

            // Try to cast the dataset pointer to the target dataset type
            _dataset = dynamic_cast<DatasetType*>(dataset);

            // Throw an exception when the pointer is null (does not match target dataset type)
            if (_dataset == nullptr)
                throw std::runtime_error("Data set ref type mismatch");
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to set dataset reference", e.what());
        }
        catch (...)
        {
            exceptionMessageBox("Unable to set dataset reference");
        }
    }

    /** Dereference operator */
    DatasetType& operator* () {
        return *_dataset;
    }

    /** Const dereference operator */
    const DatasetType& operator* () const {
      return *_dataset;
    }

    /** Arrow operator */
    DatasetType* operator-> () {
        return _dataset;
    }

    /** Arrow operator */
    const DatasetType* operator-> () const {
        return _dataset;
    }

    /** get source data */
    DatasetType& getSourceData() {
        return dynamic_cast<DatasetType&>(DataSet::getSourceData(*_dataset));
    }

    /** Get the dataset pointer */
    DatasetType* get() {
        return _dataset;
    }

    /** Get the dataset pointer of the specified type */
    template<typename TargetDatasetType>
    TargetDatasetType* get() {
        return dynamic_cast<TargetDatasetType*>(_dataset);
    }

    /** Returns whether the dataset pointer is valid (if the dataset actually exists) */
    bool isValid() const {
        return _dataset != nullptr;
    }

    /** Get the current dataset globally unique identifier */
    QString getDatasetId() const {
        return _datasetId;
    }

    /** Resets the internals (dataset pointer to nullptr etc.) */
    void reset() {
        _dataset    = nullptr;
        _datasetId  = "";
    }

private:
    QString         _datasetId;     /** Globally unique dataset identifier */
    DatasetType*    _dataset;       /** Pointer to dataset (if any) */
};

}
}
