#pragma once

#include "Plugin.h"

#include "DataHierarchyItem.h"
#include "Set.h"

namespace hdps
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
    TransformationPlugin(const PluginFactory* factory) :
        Plugin(factory),
        _inputDatasets()
    {
    }

    /** Destructor */
    ~TransformationPlugin() = default;

    /**
     * Set input dataset smart pointers
     * @param inputDatasets Smart pointers to the input dataset
     */
    void setInputDatasets(Datasets inputDatasets) {
        _inputDatasets = inputDatasets;
    }

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset(std::int32_t datasetIndex) {
        if (datasetIndex >= _inputDatasets.count())
            return Dataset<DatasetType>();

        return _inputDatasets[datasetIndex].get<DatasetType>();
    }

    /** The method which handles the data transformation */
    virtual void transform() = 0;

protected:
    Datasets    _inputDatasets;       /** Input dataset smart pointers */
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
    TransformationPluginFactory() :
        PluginFactory(Type::TRANSFORMATION)
    {
    }

    /** Destructor */
    ~TransformationPluginFactory() = default;
    
    /** Returns the plugin icon */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("random");
    }

    /** Produces an instance of a transformation plugin */
    TransformationPlugin* produce() override = 0;
};

}

}

Q_DECLARE_INTERFACE(hdps::plugin::TransformationPluginFactory, "hdps.TransformationPluginFactory")
