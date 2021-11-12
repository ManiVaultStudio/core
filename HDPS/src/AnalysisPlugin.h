#pragma once

#include "Plugin.h"

#include "DataHierarchyItem.h"

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
     * Set input dataset
     * @param inputDataset Reference to input dataset
     */
    void setInputDataset(DataSet& inputDataset) {
        _input.set(&inputDataset);
    }

    /** Get input dataset */
    template<typename DatasetType = DataSet>
    DatasetType& getInputDataset() {
        return dynamic_cast<DatasetType&>(*_input);
    }

    /**
     * Set output dataset
     * @param outputDataset Reference to output dataset
     */
    void setOutputDataset(DataSet& outputDataset) {
        _output.set(&outputDataset);
    }

    /** Get output dataset */
    template<typename DatasetType = DataSet>
    DatasetType& getOutputDataset() {
        return dynamic_cast<DatasetType&>(*_output);
    }

protected: // Status

    /** Get task status */
    DataHierarchyItem::TaskStatus getTaskStatus() const {
        if (!_output.isValid())
            return DataHierarchyItem::TaskStatus::Undefined;

        return _output->getDataHierarchyItem().getTaskStatus();
    }

    /**
     * Set name of the task
     * @param taskName Name of the task
     */
    void setTaskName(const QString& taskName) {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskName(taskName);
    }

    /**
     * Set the task progress
     * @param taskProgress Progress of the task (%)
     */
    void setTaskProgress(const float& taskProgress) {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskProgress(taskProgress);
    }

    /**
     * Set the task description
     * @param taskDescription Description of the task
     */
    void setTaskDescription(const QString& taskDescription) {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskDescription(taskDescription);
    }

    /** Set the task status to running */
    void setTaskRunning() {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskRunning();
    }

    /** Set the task status to finished */
    void setTaskFinished() {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskFinished();
    }

    /** Set the task status to aborted */
    void setTaskAborted() {
        if (!_output.isValid())
            return;

        _output->getDataHierarchyItem().setTaskAborted();
    }

protected:
    util::DatasetRef<DataSet>     _input;       /** Input dataset */
    util::DatasetRef<DataSet>     _output;      /** Output dataset */
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
