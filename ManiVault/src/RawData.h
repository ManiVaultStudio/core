// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Plugin.h"
#include "DataType.h"
#include "Application.h"

#include <QString>

namespace mv {
    class DatasetImpl;

namespace plugin {

class CORE_EXPORT RawData : public Plugin
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

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    virtual Dataset<DatasetImpl> createDataSet(const QString& guid = "") const = 0;

    /**
     * Get amount of data occupied by the raw data
     * @return Size of the raw data in bytes
     */
    virtual std::uint64_t getRawDataSize() const {
//#ifdef _DEBUG
//        qDebug() << __FUNCTION__ << "not implemented in derived class";
//#endif

        return {};
    }

private:
    DataType    _dataType;  /** Type of data */
};

class CORE_EXPORT RawDataFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    RawDataFactory() :
        PluginFactory(Type::DATA, "Raw data")
    {
        setIconByName("table");
        setCategoryIconByName("table");
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

Q_DECLARE_INTERFACE(mv::plugin::RawDataFactory, "ManiVault.RawDataFactory")
