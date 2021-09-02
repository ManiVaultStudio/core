#pragma once

#include "event/EventListener.h"

#include "Set.h"

#include <QString>

#include <stdexcept>

namespace hdps
{

class DataSet;
class CoreInterface;

namespace util
{

/**
 * Dataset reference private class
 * 
 * Class for internal use only. It enables a shared pointer to the core
 * for all templated dataset reference classes
 *
 * @author T. Kroes
 */
class DatasetRefPrivate : public QObject, public EventListener
{
Q_OBJECT

protected:

    /**
     * Constructor
     * @param datasetName Name of the dataset
     * @param parent Pointer to parent object
     */
    DatasetRefPrivate(const QString& datasetName, QObject* parent = nullptr);

public:

    /**
     * Set the core to which this dataset reference listens for events (dataset removal)
     * @param core Pointer to the core
     */
    static void setCore(CoreInterface* core);

protected:

    /** Pointer to core interface */
    static CoreInterface* core;

    template<typename> friend class DatasetRef;

signals:

    /**
     * Signals that the dataset pointer changed
     * @param datasetName Name of the dataset
     */
    void changed(const QString& datasetName);

    /** Signals that the dataset pointer became invalid (nullptr) */
    void invalidated();
};

/**
 * Dataset reference class
 *
 * Smart dataset pointer for datasets
 * 
 * This smart pointer ensures that access to datasets through pointers 
 * is more secure and requires less code. It internally manages a pointer
 * throughout the lifetime of a dataset (resets the datasets to nullptr
 * when a dataset is removed etc.). There is no need to manually request
 * a pointer to a dataset as the pointer is resolved by the smart pointer
 * by dataset name.
 *
 * @author T. Kroes
 */
template<typename DatasetType>
class DatasetRef : public DatasetRefPrivate
{
public:

    /**
     * Constructor
     * @param datasetName Name of the dataset that will be used to assign the internal dataset pointer
     * @param parent Pointer to parent object
     */
    DatasetRef(const QString& datasetName, QObject* parent = nullptr) :
        DatasetRefPrivate(datasetName, parent),
        _dataset(nullptr)
    {
        // Register for data removal events
        registerDataEvent([this](hdps::DataEvent* dataEvent) {
            if (_dataset == nullptr)
                return;

            if (dataEvent->getType() != EventType::DataRemoved)
                return;

            if (dataEvent->dataSetName != _datasetName)
                return;

            _dataset = nullptr;

            emit invalidated()
        });

        setDatasetName(datasetName);
    }

    /** Dereference operator */
    DatasetType& operator* ()
    {
        return *_dataset;
    }

    /** Arrow operator */
    DatasetType* operator-> ()
    {
        return _dataset;
    }

    /** Returns whether the dataset this reference points to is alive */
    bool isAlive() const
    {
        return _dataset != nullptr;
    }

    /**
     * Set the name of the current dataset
     * @param datasetName Name of the dataset for which the pointer needs to be resolved
     */
    void setDatasetName(const QString& datasetName)
    {
        Q_ASSERT(!datasetName.isEmpty());
        Q_ASSERT(core != nullptr);

        if (datasetName.isEmpty() || core != nullptr)
            return;

        _datasetName = datasetName;

        try
        {
            // Request dataset from the core
            auto& dataset = core->requestData(datasetName);
            
            // Try to cast the dataset pointer to the target dataset type
            _dataset = dynamic_cast<DatasetType*>(&dataset);

            // Throw an exception when the pointer is null (does not match target dataset type)
            if (_dataset == nullptr)
                throw std::runtime_error(QString("Data set ref type mismatch, %1 is of different dataset type").arg(datasetName));

            // Let subscribers know that the pointer changed
            emit changed(_datasetName);
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
        }
    }

private:
    QString         _datasetName;   /** The name of the dataset to which the reference points */
    DatasetType*    _dataset;       /** Pointer to dataset (if any) */
};

}
}
