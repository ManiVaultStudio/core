#ifndef HDPS_RAWDATA_H
#define HDPS_RAWDATA_H

#include "PluginFactory.h"

#include <QString>

namespace hdps {
    class DataSet;

    class DataType
    {
    public:
        DataType() = delete;

        DataType(QString type) :
            _type(type)
        { }

        inline friend bool operator==(const DataType& lhs, const DataType& rhs) { return lhs._type == rhs._type; }
        inline friend bool operator!=(const DataType& lhs, const DataType& rhs) { return !(lhs == rhs); }
        inline friend QString operator+(const QString& lhs, const DataType& rhs) { return lhs + rhs._type; }

    private:
        QString _type;

        friend class RawData;
        friend class Core;
    };

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
