#ifndef SCATTERPLOT_PLUGIN_H
#define SCATTERPLOT_PLUGIN_H

#include <ViewPlugin.h>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ScatterplotWidget;

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin() : ViewPlugin("Scatterplot View") { }
    ~ScatterplotPlugin(void);
    
    void init();
    
    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved() Q_DECL_OVERRIDE;
private:
    ScatterplotWidget* widget;
};


// =============================================================================
// Factory
// =============================================================================

class ScatterplotPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ScatterplotPlugin"
                      FILE  "ScatterplotPlugin.json")
    
public:
    ScatterplotPluginFactory(void) {}
    ~ScatterplotPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // SCATTERPLOT_PLUGIN_H
