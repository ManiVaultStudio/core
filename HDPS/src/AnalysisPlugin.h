#pragma once

#include "PluginFactory.h"

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
        _inputDatasetName()
    {
    }

    ~AnalysisPlugin() override {};

    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("chart-line");
    }

    QString getInputDatasetName() const {
        return _inputDatasetName;
    }

    void setInputDatasetName(const QString& inputDatasetName) {
        Q_ASSERT(!inputDatasetName.isEmpty());

        _inputDatasetName = inputDatasetName;
    }

    QString getOutputDatasetName() const {
        return _outputDatasetName;
    }

protected:

    void notifyStarted() {
        qApp->processEvents();

        _core->getDataHierarchyItem(_outputDatasetName).setDescription("");
        _core->getDataHierarchyItem(_outputDatasetName).setProgress(0.0);
    }

    void notifyProgressSection(const QString& section) {
        qApp->processEvents();

        _core->getDataHierarchyItem(_outputDatasetName).setDescription(section);
    }

    void notifyProgressPercentage(const float& percentage) {
        qApp->processEvents();

        _core->getDataHierarchyItem(_outputDatasetName).setProgress(percentage);
    }

    void notifyFinished() {
        qApp->processEvents();

        _core->getDataHierarchyItem(_outputDatasetName).setDescription("");
        _core->getDataHierarchyItem(_outputDatasetName).setProgress(0.0);
    }

    void notifyAborted(const QString& reason) {
        qApp->processEvents();

        _core->getDataHierarchyItem(_outputDatasetName).setDescription("");
        _core->getDataHierarchyItem(_outputDatasetName).setProgress(0.0);
    }

protected:
    QString   _inputDatasetName;        /** Name of the input dataset */
    QString   _outputDatasetName;       /** Name of the output dataset */
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