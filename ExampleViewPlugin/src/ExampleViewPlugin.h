#pragma once

#include <ViewPlugin.h>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ExampleViewPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ExampleViewPlugin() : ViewPlugin("Example View") { }
    ~ExampleViewPlugin(void);
    
    void init();
    
    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
};


// =============================================================================
// Factory
// =============================================================================

class ExampleViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ExampleViewPlugin"
                      FILE  "ExampleViewPlugin.json")
    
public:
    ExampleViewPluginFactory(void) {}
    ~ExampleViewPluginFactory(void) {}
    
    ViewPlugin* produce();
};
