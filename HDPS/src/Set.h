#pragma once

#include "RawData.h"

#include "CoreInterface.h"

#include <QString>
#include <QVector>
#include <memory>

namespace hdps
{

class DataSet
{
public:
    DataSet(CoreInterface* core, QString dataName) :
        _core(core),
        _dataName(dataName),
        _all(false),
        _rawData(nullptr)
    {
        
    }

    virtual ~DataSet() {}

    virtual DataSet* copy() const = 0;

    virtual void createSubset() const = 0;

    QString getName() const
    {
        return _name;
    }

    bool isFull() const
    {
        return _all;
    }

    void setName(QString name)
    {
        _name = name;
    }

    void setAll()
    {
        _all = true;
    }

    bool isDerivedData() const
    {
        return _derived;
    }

    /**
     * Returns the data type of the raw data associated with this dataset.
     */
    DataType getDataType() const
    {
        return _core->requestRawData(getDataName()).getDataType();
    }

    /**
     * If the given set is derived from another set, then calls this function on the other set.
     * If the given set is not derived from another set, then returns the given set.
     */
    template <class T>
    static T& getSourceData(T& set)
    {
        return set.isDerivedData() ? getSourceData(static_cast<T&>(set._core->requestData(set._sourceSetName))) : static_cast<T&>(set._core->requestData(set._name));
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection associated with this data set
     */
    DataSet& getSelection() const
    {
        return _core->requestSelection(getSourceData(*this).getDataName());
    }

    void setSelection(std::vector<unsigned int> indices)
    {

    }

    // Should be protected, but can't because of DataManager needing access to the data
    QString getDataName() const
    {
        return _dataName;
    }

protected:
    template <class DataType>
    DataType& getRawData() const
    {
        return dynamic_cast<DataType&>(_core->requestRawData(getDataName()));
    }

    CoreInterface* _core;
private:
    std::unique_ptr<RawData> _rawData;

    QString _name;
    QString _dataName;
    bool _all;

    bool _derived = false;
    QString _sourceSetName;

    friend class Core;
};

} // namespace hdps
