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

/**
 * Transformation plugin base class
 *
 * @author T. Kroes
 */
class TransformationPlugin : public Plugin
{
public:

    /**
     * Constructor
     * @param factory Pointer to transformation plugin factory
     */
    TransformationPlugin(const PluginFactory* factory);

    /** Performs the data transformation */
    virtual void transform() = 0;

    /**
     * Get input datasets
     * @return Input datasets
     */
    Datasets getInputDatasets() const;
    
    /**
     * Get first input dataset
     * @return First input dataset
     */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        if (_inputDatasets.size() > 0)
            return Dataset<DatasetType>(_inputDatasets[0].get<DatasetType>());
        else
            return Dataset<DatasetImpl>();
    }

    /**
     * Set input datasets
     * @inputDatasets Input datasets
     */
    void setInputDatasets(const Datasets& inputDatasets);

    /**
     * Set a single input dataset
     * @param inputDataset Smart pointer to the input dataset
     */
    void setInputDataset(const Dataset<DatasetImpl>& inputDataset);

private:
    Datasets    _inputDatasets;        /** One, or more, input dataset */
};

/**
 * Transformation plugin factory class
 *
 * @author T. Kroes
 */
class TransformationPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:

    /** Default constructor */
    TransformationPluginFactory();

    /** Destructor */
    ~TransformationPluginFactory() = default;
    
    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /** Produces an instance of a transformation plugin */
    TransformationPlugin* produce() override = 0;
};

}

}

Q_DECLARE_INTERFACE(mv::plugin::TransformationPluginFactory, "ManiVault.TransformationPluginFactory")
