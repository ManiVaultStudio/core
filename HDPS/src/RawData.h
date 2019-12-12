#pragma once

#include "PluginFactory.h"

namespace hdps {
    class DataSet;

    class DataType
    {
    public:
        DataType(QString type) :
            _type(type)
        { }

        inline friend bool operator==(const DataType& lhs, const DataType& rhs) { return lhs._type == rhs._type; }
        inline friend bool operator!=(const DataType& lhs, const DataType& rhs) { return !(lhs == rhs); }
        inline friend QString operator+(const QString& lhs, const DataType& rhs) { return lhs + rhs._type; }

    private:
        QString _type;

        friend class RawData;
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
        DataType _dataType;

        bool _derived = false;

        QString _sourceDataName;
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
