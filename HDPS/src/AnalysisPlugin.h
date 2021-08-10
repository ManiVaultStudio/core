#pragma once

#include "PluginFactory.h"
#include "DataHierarchyItem.h"

#include <memory>

namespace hdps
{

namespace plugin
{

class AnalysisPlugin : public Plugin
{

public:
    AnalysisPlugin(QString name) :
        Plugin(Type::ANALYSIS, name),
        _inputDataHierarchyItem(nullptr),
        _outputDataHierarchyItem(nullptr)
    {
    }

    ~AnalysisPlugin() override {};

    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("chart-line");
    }

    DataHierarchyItem* getInputDataHierarchyItem() const {
        return _inputDataHierarchyItem;
    }

    QString getInputDatasetName() const {
        Q_ASSERT(_inputDataHierarchyItem != nullptr);

        if (_inputDataHierarchyItem == nullptr)
            return "";

        return _inputDataHierarchyItem->getDatasetName();
    }

    /** Get input dataset */
    template<typename DatasetType>
    DatasetType& getInputDataset() const {
        return dynamic_cast<DatasetType&>(_inputDataHierarchyItem->getDataset());
    }

    void setInputDataHierarchyItem(DataHierarchyItem* inputDataHierarchyItem) {
        Q_ASSERT(inputDataHierarchyItem != nullptr);

        if (inputDataHierarchyItem == nullptr)
            return;

        _inputDataHierarchyItem = inputDataHierarchyItem;
    }

    DataHierarchyItem* getOutputDataHierarchyItem() const {
        return _outputDataHierarchyItem;
    }

    QString getOutputDatasetName() const {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return "";

        return _outputDataHierarchyItem->getDatasetName();
    }

    /** Get output dataset */
    template<typename DatasetType>
    DatasetType& getOutputDataset() const {
        return dynamic_cast<DatasetType&>(_outputDataHierarchyItem->getDataset());
    }

    void setOutputDatasetName(const QString& outputDatasetName) {
        Q_ASSERT(!outputDatasetName.isEmpty());

        if (outputDatasetName.isEmpty())
            return;

        _outputDataHierarchyItem = _core->getDataHierarchyItem(outputDatasetName);
    }

protected: // Status

    /** Get task status */
    DataHierarchyItem::TaskStatus getTaskStatus() const {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        return _outputDataHierarchyItem->getTaskStatus();
    }

    /**
     * Set name of the task
     * @param taskName Name of the task
     */
    void setTaskName(const QString& taskName) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskName(taskName);
    }

    /**
     * Set the task progress
     * @param taskProgress Progress of the task (%)
     */
    void setTaskProgress(const float& taskProgress) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskProgress(taskProgress);
    }

    /**
     * Set the task description
     * @param taskDescription Description of the task
     */
    void setTaskDescription(const QString& taskDescription) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskDescription(taskDescription);
    }

    /** Set the task status to running */
    void setTaskRunning() {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskRunning();
    }

    /** Set the task status to finished */
    void setTaskFinished() {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskFinished();
    }

    /** Set the task status to aborted */
    void setTaskAborted() {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        _outputDataHierarchyItem->setTaskAborted();
    }

protected:
    DataHierarchyItem*  _inputDataHierarchyItem;        /** Input data hierarchy item */
    DataHierarchyItem*  _outputDataHierarchyItem;       /** Output data hierarchy item */
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~AnalysisPluginFactory() override {};
    
    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "cytosplore.AnalysisPluginFactory")