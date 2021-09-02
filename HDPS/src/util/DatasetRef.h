#pragma once

#include "event/EventListener.h"

#include "DataHierarchyItem.h"
#include "Set.h"
#include "Application.h"

#include <QString>
#include <QMessageBox>

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
     * @param parent Pointer to parent object
     */
    DatasetRefPrivate(QObject* parent = nullptr);

    template<typename> friend class DatasetRef;

signals:

    /**
     * Signals that the dataset pointer changed
     * @param datasetName Name of the dataset
     */
    void changed(const QString& datasetName);

    /** Signals that the dataset pointer became invalid (nullptr) */
    void invalidated();

    /**
     * Signals that the name of the currently pointed to dataset changed
     * @param oldDatasetName Old name of the dataset
     * @param newDatasetName New name of the dataset
     */
    void datasetNameChanged(const QString& oldDatasetName, const QString& newDatasetName);
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
        _dataset(nullptr)
    {
        // Register for data removal events
        registerDataEvent([this](hdps::DataEvent* dataEvent) {
            if (_dataset == nullptr)
                return;

            switch (dataEvent->getType()) {
                case EventType::DataRemoved:
                {
                    if (dataEvent->dataSetName != _datasetName)
                        return;

                    _dataset = nullptr;

                    emit invalidated();

                    break;
                }

                case EventType::DataRenamed:
                {
                    auto datasetRenamedEvent = static_cast<hdps::DataRenamedEvent*>(dataEvent);

                    _datasetName = datasetRenamedEvent->dataSetName;

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

    /** Arrow operator */
    DatasetType* operator-> () {
        return _dataset;
    }

    /** Arrow operator */
    const DatasetType* operator-> () const {
        return _dataset;
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
        Q_ASSERT(Application::core() != nullptr);

        if (Application::core() == nullptr)
            return;

        if (datasetName.isEmpty()) {
            reset();
            return;
        }
        
        if (datasetName == _datasetName)
            return;

        const auto previousName = _datasetName;

        _datasetName = datasetName;

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

            // Let subscribers know that the pointer changed
            emit changed(_datasetName);

            // Let subscribers know that the dataset name changed
            emit datasetNameChanged(previousName, _datasetName);
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
        }
    }

    /** Resets the internals (dataset pointer to nullptr etc.) */
    void reset() {
        _datasetName = "";
        _dataset = nullptr;
    }

    /** Get the corresponding data hierarchy item */
    DataHierarchyItem* getDataHierarchyItem() {
        Q_ASSERT(Application::core() != nullptr);

        if (!isValid())
            return nullptr;

        return Application::core()->getDataHierarchyItem(_datasetName);
    }

    /** Get the corresponding data hierarchy item */
    const DataHierarchyItem* getDataHierarchyItem() const {
        return const_cast<DatasetRef<DatasetType>*>(this)->getDataHierarchyItem();
    }

    /**
     * Informs the core that the data has changed
     * Consumers of this class don't need to keep a pointer
     * to the core anymore to inform others that data changed
     */
    void notifyDataChanged() {
        Q_ASSERT(Application::core() != nullptr);

        if (Application::core() == nullptr)
            return;

        Application::core()->notifyDataChanged(_datasetName);
    }

    /**
     * Informs the core that the data has changed
     * Consumers of this class don't need to keep a pointer
     * to the core anymore to inform others that data changed
     */
    void notifySelectionChanged() {
        Q_ASSERT(Application::core() != nullptr);

        if (Application::core() == nullptr)
            return;

        Application::core()->notifySelectionChanged(_datasetName);
    }

private:
    QString         _datasetName;   /** The name of the dataset to which the reference points */
    DatasetType*    _dataset;       /** Pointer to dataset (if any) */
};

}
}
