#pragma once

#include "Plugin.h"

#include "DataHierarchyItem.h"
#include "Set.h"

#include <memory>

namespace hdps
{

namespace plugin
{

class AnalysisPlugin : public Plugin
{

public:
    AnalysisPlugin(const PluginFactory* factory) :
        Plugin(factory),
        _input(),
        _output()
    {
    }

    ~AnalysisPlugin() override {};

    /**
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to the input dataset
     */
    void setInputDataset(Dataset<DatasetImpl>& inputDataset) {
        _input = inputDataset;
    }

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        return Dataset<DatasetType>(_input.get<DatasetType>());
    }

    /**
     * Set output dataset smart pointer
     * @param outputDataset Smart pointer to output dataset
     */
    void setOutputDataset(Dataset<DatasetImpl>& outputDataset) {
        _output = outputDataset;
    }

    /** Get output dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getOutputDataset() {
        return Dataset<DatasetType>(_output.get<DatasetType>());
    }

protected:
    Dataset<DatasetImpl>    _input;       /** Input dataset smart pointer */
    Dataset<DatasetImpl>    _output;      /** Output dataset smart pointer */
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    AnalysisPluginFactory() :
        PluginFactory(Type::ANALYSIS)
    {

    }
    ~AnalysisPluginFactory() override {};
    
    /** Returns the plugin icon */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("square-root-alt");
    }

    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "hdps.AnalysisPluginFactory")
