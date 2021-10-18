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
     * @param parent Pointer to parent object
     */
    DatasetRefPrivate(QObject* parent = nullptr);

    template<typename> friend class DatasetRef;

signals:

    /**
     * Signals that the name of the currently pointed to dataset changed
     * @param oldDatasetName Old name of the dataset
     * @param newDatasetName New name of the dataset
     */
    void datasetNameChanged(const QString& oldDatasetName, const QString& newDatasetName);

    /**
     * Signals that the dataset is about to be removed
     * @param datasetName Name of the dataset that is about to be removed
     */
    void datasetAboutToBeRemoved(const QString& datasetName);

    /**
     * Signals that the dataset has been removed
     * @param datasetName Name of the dataset that was removed
     */
    void datasetRemoved(const QString& datasetName);
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
    DatasetRef(const QString& datasetName = "", QObject* parent = nullptr) :
        DatasetRefPrivate(parent),
        _datasetName(),
        _dataset(nullptr)
    {
        // Register for data removal events
        registerDataEvent([this](hdps::DataEvent* dataEvent) {
            if (_dataset == nullptr)
                return;

            switch (dataEvent->getType()) {
                case EventType::DataAboutToBeRemoved:
                {
                    if (dataEvent->dataSetName != _datasetName)
                        return;

                    _dataset = nullptr;

                    emit datasetAboutToBeRemoved(dataEvent->dataSetName);

                    break;
                }

                case EventType::DataRemoved:
                {
                    const auto previousDatasetName = _datasetName;

                    if (dataEvent->dataSetName != _datasetName)
                        return;

                    _dataset        = nullptr;
                    _datasetName    = "";

                    emit datasetRemoved(previousDatasetName);
                    emit datasetNameChanged(previousDatasetName, "");

                    break;
                }

                case EventType::DataRenamed:
                {
                    // Get data renamed event
                    auto datasetRenamedEvent = static_cast<hdps::DataRenamedEvent*>(dataEvent);

                    // No need to process zero delta
                    if (datasetRenamedEvent->oldName != _datasetName)
                        return;

                    // Update with new name
                    _datasetName = datasetRenamedEvent->dataSetName;

                    // Notify others of the name change
                    emit datasetNameChanged(datasetRenamedEvent->oldName, datasetRenamedEvent->dataSetName);
                    break;
                }
            }
        });

        setDatasetName(datasetName);
    }

    /** Dereference operator */
    DatasetType& operator* () {
        return *_dataset;
    }

    /** Const dereference operator */
    const DatasetType& operator* () const {
      return *_dataset;
    }
    
    /** Dereference operator */
    DatasetType& operator* () const {
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

    /** Get the current dataset name */
    QString getDatasetName() const {
        return _datasetName;
    }

    /**
     * Set the name of the current dataset
     * @param datasetName Name of the dataset for which the pointer needs to be resolved 
     */
    void setDatasetName(const QString& datasetName) {
        if (datasetName.isEmpty()) {
            reset();
            return;
        }
        
        // No need to process the same name
        if (datasetName == _datasetName)
            return;

        // Assign the new dataset name
        _datasetName = datasetName;

        // Cache the previous dataset name
        const auto previousDatasetName = _datasetName;

        try
        {
            // Request dataset from the core
            auto& dataset = Application::core()->requestData(datasetName);
            
            // Try to cast the dataset pointer to the target dataset type
            _dataset = dynamic_cast<DatasetType*>(&dataset);

            // Throw an exception when the pointer is null (does not match target dataset type)
            if (_dataset == nullptr) {

                const auto errorMessage = QString("Data set ref type mismatch, %1 is of different dataset type").arg(datasetName);
                throw std::runtime_error(errorMessage.toLatin1());
            }

            // Let subscribers know that the dataset name changed
            emit datasetNameChanged(previousDatasetName, _datasetName);
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

    /** Resets the internals (dataset pointer to nullptr etc.) */
    void reset() {
        // Cache the previous dataset name
        const auto previousDatasetName = _datasetName;

        _dataset        = nullptr;
        _datasetName    = "";

        // Let subscribers know that the dataset name changed
        emit datasetNameChanged(previousDatasetName, "");
    }

    /**
     * Informs the core that the data has changed
     * Consumers of this class don't need to keep a pointer
     * to the core anymore to inform others that data changed
     */
    void notifyDataChanged() {
        Application::core()->notifyDataChanged(_datasetName);
    }

    /**
     * Informs the core that the data has changed
     * Consumers of this class don't need to keep a pointer
     * to the core anymore to inform others that data changed
     */
    void notifySelectionChanged() {
        if (_dataset == nullptr)
            return;

        Application::core()->notifySelectionChanged(_datasetName);
    }

private:
    QString         _datasetName;   /** The name of the dataset to which the reference points */
    DatasetType*    _dataset;       /** Pointer to dataset (if any) */
};

}
}
