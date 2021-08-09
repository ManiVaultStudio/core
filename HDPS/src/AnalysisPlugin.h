#pragma once

#include "PluginFactory.h"
#include "DataHierarchyItem.h"

#include <memory>

class QWidget;

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

protected:

    void notifyStarted() {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return;

        qApp->processEvents();

        _outputDataHierarchyItem->setDescription("");
        _outputDataHierarchyItem->setProgress(0.0);
    }

    void notifyProgressSection(const QString& section) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return;

        qApp->processEvents();

        _outputDataHierarchyItem->setDescription(section);
    }

    void notifyProgressPercentage(const float& percentage) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return;

        qApp->processEvents();

        _outputDataHierarchyItem->setProgress(percentage);
    }

    void notifyFinished() {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return;

        qApp->processEvents();

        _outputDataHierarchyItem->setDescription("");
        _outputDataHierarchyItem->setProgress(0.0);
    }

    void notifyAborted(const QString& reason) {
        Q_ASSERT(_outputDataHierarchyItem != nullptr);

        if (_outputDataHierarchyItem == nullptr)
            return;

        qApp->processEvents();

        _outputDataHierarchyItem->setDescription("");
        _outputDataHierarchyItem->setProgress(0.0);
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