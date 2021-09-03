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

    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("chart-line");
    }

    /** Get input dataset name */
    QString getInputDatasetName() const {
        return _input.getDatasetName();
    }

    /**
     * Set input dataset name (updates the input data reference)
     * @param inputDatasetName Name of the input dataset
     */
    void setInputDatasetName(const QString& inputDatasetName) {
        _input.setDatasetName(inputDatasetName);
    }

    /** Get input dataset */
    template<typename DatasetType>
    DatasetType& getInputDataset() {
        return dynamic_cast<DatasetType&>(*_input);
    }

    /** Get output dataset name */
    QString getOutputDatasetName() {
        return _output.getDatasetName();
    }

    /** Get output dataset name */
    const QString getOutputDatasetName() const {
        return _output.getDatasetName();
    }

    /**
     * Set output dataset name (updates the output data reference)
     * @param outputDatasetName Name of the output dataset
     */
    void setOutputDatasetName(const QString& outputDatasetName) {
        _output.setDatasetName(outputDatasetName);
    }

    /** Get output dataset */
    template<typename DatasetType>
    DatasetType& getOutputDataset() {
        return dynamic_cast<DatasetType&>(*_output);
    }

protected: // Status

    /** Get task status */
    DataHierarchyItem::TaskStatus getTaskStatus() const {
        if (!_output.isValid())
            return DataHierarchyItem::TaskStatus::Undefined;

        return _output->getHierarchyItem().getTaskStatus();
    }

    /**
     * Set name of the task
     * @param taskName Name of the task
     */
    void setTaskName(const QString& taskName) {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskName(taskName);
    }

    /**
     * Set the task progress
     * @param taskProgress Progress of the task (%)
     */
    void setTaskProgress(const float& taskProgress) {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskProgress(taskProgress);
    }

    /**
     * Set the task description
     * @param taskDescription Description of the task
     */
    void setTaskDescription(const QString& taskDescription) {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskDescription(taskDescription);
    }

    /** Set the task status to running */
    void setTaskRunning() {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskRunning();
    }

    /** Set the task status to finished */
    void setTaskFinished() {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskFinished();
    }

    /** Set the task status to aborted */
    void setTaskAborted() {
        if (!_output.isValid())
            return;

        _output->getHierarchyItem().setTaskAborted();
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
    
    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "hdps.AnalysisPluginFactory")
