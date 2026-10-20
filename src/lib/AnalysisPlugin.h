// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Plugin.h"

#include "Set.h"

namespace mv
{

namespace plugin
{

class CORE_EXPORT AnalysisPlugin : public Plugin
{

public:
    AnalysisPlugin(const PluginFactory* factory);

    ~AnalysisPlugin() override = default;

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name);

    /**
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to the input dataset
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

    /**
     * Set output dataset smart pointer
     * @param outputDataset Smart pointer to output dataset
     */
    void setOutputDataset(const Dataset<DatasetImpl>& outputDataset);

    /**
     * Set output datasets smart pointers
     * @param outputDatasets Vector of smart pointers to output datasets
     */
    void setOutputDatasets(const Datasets& outputDatasets);

    /** Get output dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getOutputDataset() {
        if (_output.size() > 0)
            return Dataset<DatasetType>(_output[0].get<DatasetType>());
        else
            return Dataset<DatasetImpl>();
    }

    /** Get output dataset smart pointers */
    Datasets getOutputDatasets() {
        return _output;
    }

public: // Serialization

    /**
     * Load analysis plugin from variant
     * @param Variant representation of the analysis plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save analysis plugin to variant
     * @param Variant representation of the analysis plugin
     */
    QVariantMap toVariantMap() const override;

protected:
    /** Returns whether any output dataset is given */
    bool outputDataInit() const { return _output.size() > 0; }

protected:
    Datasets    _input;       /** Input datasets smart pointers */
    Datasets    _output;      /** Output datasets smart pointers */
};

class CORE_EXPORT AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    AnalysisPluginFactory() :
        PluginFactory(Type::ANALYSIS, "Analysis")
    {
        setIconByName("square-root-variable");
        setCategoryIconByName("square-root-variable");
    }

    ~AnalysisPluginFactory() override = default;

    AnalysisPlugin* produce() override = 0;
};

}

}

Q_DECLARE_INTERFACE(mv::plugin::AnalysisPluginFactory, "ManiVault.AnalysisPluginFactory")
