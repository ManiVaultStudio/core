#pragma once

#include "Plugin.h"
#include "DataType.h"

#include <QString>

namespace hdps {
    class DatasetImpl;

namespace plugin {

class RawData : public Plugin
{
public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     * @param dataType Type of data
     */
    RawData(const PluginFactory* factory, const DataType& dataType) :
        Plugin(factory),
        _dataType(dataType)
    {
    }

    /** Destructor */
    ~RawData() override
    {
    }

    /** Get the type of raw data */
    const DataType& getDataType() const {
        return _dataType;
    }

    /** Create dataset for raw data */
    virtual Dataset<DatasetImpl> createDataSet() const = 0;

private:
    DataType    _dataType;  /** Type of data */
};

class RawDataFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    RawDataFactory() :
        PluginFactory(Type::DATA)
    {

    }
    ~RawDataFactory() override {};
    
    /**
     * Produces the plugin
     * @return Pointer to the produced plugin
     */
    RawData* produce() override = 0;
};

}
}

Q_DECLARE_INTERFACE(hdps::plugin::RawDataFactory, "hdps.RawDataFactory")
