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

template<typename DatasetType>
class DataRef : public EventListener
{
public:
    DataRef(const QString& datasetName) :
        EventListener(),
        _dataset(nullptr)
    {
        Q_ASSERT(core != nullptr);

        setEventCore(core);

        registerDataEvent([this](hdps::DataEvent* dataEvent) {
            if (_dataset == nullptr)
                return;

            if (dataEvent->getType() != EventType::DataRemoved)
                return;

            if (dataEvent->dataSetName != _dataset->getName())
                return;

            _dataset = nullptr;
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
     * @param datasetName Name of the dataset
     */
    void setDatasetName(const QString& datasetName)
    {
        Q_ASSERT(!datasetName.isEmpty());
        Q_ASSERT(core != nullptr);

        try
        {
            auto& dataset = core->requestData(datasetName);
            
            _dataset = dynamic_cast<DatasetType*>(&);

            if (_dataset == nullptr)
                throw std::runtime_error(QString("Data ref type mismatch, %1 is of different dataset type").arg(datasetName));
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("HDPS"), e.what(), QMessageBox::Ok);
        }
    }

protected:

    /**
     * Set the core to which this dataset reference listens for events
     * @param core Pointer to the core
     */
    void setCore(CoreInterface* core)
    {
        DataRef::core = core;
    }

private:
    DatasetType*    _dataset;   /** Pointer to dataset (if any) */

    /** Pointer to core interface */
    static CoreInterface* core;     
};

template<typename DatasetType>
CoreInterface* DataRef<DatasetType>::core = nullptr;

}
}