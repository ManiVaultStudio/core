#ifndef HDPS_RAWDATA_H
#define HDPS_RAWDATA_H

#include "Plugin.h"
#include "DataType.h"

#include <QString>

namespace hdps {
    class DataSet;

namespace plugin {
    class RawData : public Plugin
    {
    public:
        RawData(const PluginFactory* factory, DataType dataType) :
            Plugin(factory),
            _dataType(dataType)
        { }

        ~RawData() override {};

        const DataType& getDataType() const
        {
            return _dataType;
        }

        virtual DataSet* createDataSet() const = 0;

    private:
        DataType _dataType;
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
    
        RawData* produce() override = 0;

        // Override supportedDataTypes function, so custom data types don't need to supply it.
        virtual DataTypes supportedDataTypes() const override { return DataTypes(); }
    };

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::RawDataFactory, "hdps.RawDataFactory")

#endif // HDPS_RAWDATA_H
