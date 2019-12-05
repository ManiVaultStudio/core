#pragma once

#include "PluginFactory.h"

namespace hdps {
    class DataSet;

namespace plugin {

class RawData : public Plugin
{
public:
    RawData(QString name) : Plugin(Type::DATA, name) { }
    ~RawData() override {};

    ///**
    // * Gets the selection associated with this data set. If the data set is
    // * derived then the selection of the source data will be returned. Otherwise,
    // * the selection of the set's data will be returned.
    // *
    // * @return The selection associated with this data set
    // */
    //DataSet& getSelection() const
    //{
    //    return isDerivedData() ? _core->requestSelection(_sourceDataName) : _core->requestSelection(getName());
    //}

    virtual DataSet* createDataSet() const = 0;

    bool isDerivedData() const
    {
        return _derived;
    }

    /**
     * Returns the raw data this data is derived from. If this data isn't
     * derived data then calling this function will throw an exception.
     *
     * @return A reference to the raw data this data is derived from
     */
    RawData& getSourceData() const
    {
        return _core->requestRawData(_sourceDataName);
    }

    QString getSourceDataName() const
    {
        return _sourceDataName;
    }

    void setDerived(bool derived, QString sourceData)
    {
        _derived = derived;
        _sourceDataName = sourceData;
    }

private:
    bool _derived = false;

    QString _sourceDataName;
};

class RawDataFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~RawDataFactory() override {};
    
    RawData* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::RawDataFactory, "hdps.RawDataFactory")
