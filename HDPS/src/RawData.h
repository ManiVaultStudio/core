#ifndef HDPS_RAWDATA_H
#define HDPS_RAWDATA_H

#include "PluginFactory.h"
#include "DataType.h"

#include <QString>

namespace hdps {
    class DataSet;

    class RawData : public plugin::Plugin
    {
    public:
        RawData(QString name, DataType dataType) :
            Plugin(plugin::Type::DATA, name),
            _dataType(dataType)
        { }

        ~RawData() override {};

        const DataType& getDataType() const
        {
            return _dataType;
        }

        virtual DataSet* createDataSet() const = 0;

        // Override supportedDataTypes function, so custom data types don't need to supply it.
        virtual DataTypes supportedDataTypes() const override { return DataTypes(); }

    private:
        DataType _dataType;
    };

namespace plugin {

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

#endif // HDPS_RAWDATA_H
