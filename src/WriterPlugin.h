// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Plugin.h"

#include <QString>

namespace mv::plugin
{

class CORE_EXPORT WriterPlugin : public Plugin
{
public:

    /**
     * Construct with pointer to plugin factory
     * @param factory Pointer to plugin factory
     */
    WriterPlugin(const PluginFactory* factory);

    /** No need for custom destructor */
    ~WriterPlugin() override = default;

    virtual void writeData() = 0;

public:

    /**
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to input dataset
     */
    void setInputDataset(const Dataset<DatasetImpl>& inputDataset);

    /**
     * Set input datasets smart pointers
     * @param inputDatasets Vector of smart pointers to the input datasets
     */
    void setInputDatasets(const Datasets& inputDatasets);

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        if (_input.size() > 0)
            return Dataset<DatasetType>(_input[0].get<DatasetType>());
        else
            return Dataset<DatasetImpl>();
    }

    /** Get input dataset smart pointer vector */
    Datasets getInputDatasets() {
        return _input;
    }

protected:
    Datasets    _input;     /** Input datasets smart pointers */
};


class CORE_EXPORT WriterPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    WriterPluginFactory() :
        PluginFactory(Type::WRITER, "Writer")
    {
        setIconByName("file-export");
        setCategoryIconByName("file-export");
    }

    /** No need for custom destructor */
    ~WriterPluginFactory() override = default;

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name)
    {
        QObject::setObjectName("Plugins/Writer/" + name);
    }

    WriterPlugin* produce() override = 0;
};

}

Q_DECLARE_INTERFACE(mv::plugin::WriterPluginFactory, "ManiVault.WriterPluginFactory")
