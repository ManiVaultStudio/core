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
    AnalysisPlugin(QString name) : Plugin(Type::ANALYSIS, name) { }
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