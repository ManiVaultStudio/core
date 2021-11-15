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
 * Dataset reference private class
 *
 * Designed to emit signals related to changes in the dataset reference
 *
 * @author T. Kroes
 */
class DatasetRefPrivate : public QObject, public EventListener
{
    Q_OBJECT

    /** Only dataset reference template classes have access to protected members */
    template<typename> friend class DatasetRef;

protected:

    /**
     * Default constructor
     * @param parent Pointer to parent object
     */
    DatasetRefPrivate() :
        QObject()
    {
    }

    /**
     * Copy constructor
     * @param other Object to copy from
     */
    DatasetRefPrivate(const DatasetRefPrivate& other) :
        QObject()
    {
        Q_ASSERT(thread() == other.thread());
        setObjectName(other.objectName());
        blockSignals(other.signalsBlocked());
    }

    /**
     * Assignment operator
     * @param other Object to assign from
     */
    DatasetRefPrivate& operator=(DatasetRefPrivate other) {
        return *this;
    }

signals:

    /**
     * Signals that the pointer to the dataset changed
     * @param dataset Pointer to current dataset
     */
    void changed(DataSet* dataset);

    /** Signals that the dataset is about to be removed */
    void aboutToBeRemoved();

    /**
     * Signals that the dataset has been removed
     * @param datasetId Globally unique identifier of the dataset that is removed
     */
    void removed(const QString& datasetId);

    /**
     * Signals that the dataset GUI name changed
     * @param oldGuiName Old GUI name
     * @param newGuiName New GUI name
     */
    void guiNameChanged(const QString& oldGuiName, const QString& newGuiName);
};

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
template<typename SetType>
class DatasetRef : public DatasetRefPrivate
{
public:

    /**
     * (Default) constructor
     * @param dataset Pointer to dataset (if any)
     */
    DatasetRef(SetType* dataset = nullptr) :
        DatasetRefPrivate(),
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
     * @param dataset Reference to dataset
     */
    DatasetRef(SetType& dataset) :
        DatasetRefPrivate(),
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
        DatasetRefPrivate(),
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

                // Data is about to be removed
                case EventType::DataAboutToBeRemoved:
                {
                    // Notify others that the dataset is about to be removed
                    emit aboutToBeRemoved();

                    break;
                }

                // Reset the reference when the data is removed
                case EventType::DataRemoved:
                {
                    // Do not process foreign datasets
                    if (dataEvent->getDataset() != (*_dataset))
                        return;

                    // Reset the reference
                    reset();

                    // Notify others that the dataset is removed
                    emit removed(_datasetId);

                    break;
                }

                // Dataset GUI name changed
                case EventType::DataGuiNameChanged:
                {
                    // Get dataset GUI name changed event
                    auto datasetGuiNameChangedEvent = static_cast<hdps::DataGuiNameChangedEvent*>(dataEvent);

                    // Notify others of the name change
                    emit guiNameChanged(datasetGuiNameChangedEvent->getPreviousGuiName(), datasetGuiNameChangedEvent->getDataset().getGuiName());

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
            _dataset = dynamic_cast<SetType*>(dataset);

            // Throw an exception when the pointer is null (does not match target dataset type)
            if (_dataset == nullptr)
                throw std::runtime_error("Data set ref type mismatch");

            // Inform others that the pointer to the dataset changed
            emit changed(_dataset);
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

    /**
     * Set the reference from a pointer to a dataset
     * @param dataset Reference to the dataset
     */
    void set(DataSet& dataset) {
        set(&dataset);
    }

    /** Dereference operator */
    SetType& operator* () {
        return *_dataset;
    }

    /** Const dereference operator */
    const SetType& operator* () const {
      return *_dataset;
    }

    /** Arrow operator */
    SetType* operator-> () {
        return _dataset;
    }

    /** Arrow operator */
    const SetType* operator-> () const {
        return _dataset;
    }

    /** get source data */
    SetType& getSourceData() {
        return dynamic_cast<SetType&>(DataSet::getSourceData(*_dataset));
    }

    /** Get the dataset pointer */
    SetType* get() {
        return _dataset;
    }

    /** Get the dataset pointer of the specified type */
    template<typename TargetSetType>
    TargetSetType* get() {
        return dynamic_cast<TargetSetType*>(_dataset);
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
    SetType*    _dataset;       /** Pointer to dataset (if any) */
};

}
}
