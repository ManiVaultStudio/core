#pragma once

#include "PluginFactory.h"
#include "widgets/SettingsWidget.h"

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

    bool hasSettings()
    {
        return getSettings() != nullptr;
    }

    virtual gui::SettingsWidget* const getSettings() = 0;

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
        _core->notifyAnalysisEvent(AnalysisStartedEvent(this));
    }

    void notifyProgressed(const float& progress) {
        _core->notifyAnalysisEvent(AnalysisProgressedEvent(this, progress));
    }

    void notifyFinished() {
        _core->notifyAnalysisEvent(AnalysisFinishedEvent(this));
    }

    void notifyAborted(const QString& reason) {
        _core->notifyAnalysisEvent(AnalysisAbortedEvent(this, reason));
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