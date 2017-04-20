#ifndef CSPLR_TEST_VIEW_PLUGIN
#define CSPLR_TEST_VIEW_PLUGIN

#include <ViewPlugin.h>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class CSPLR_ExampleViewPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    CSPLR_ExampleViewPlugin(void);
	~CSPLR_ExampleViewPlugin(void);
    
    void init();
    
    void dataAdded(void);
    void dataRefreshed(void);
};


// =============================================================================
// Factory
// =============================================================================

class CSPLR_ExampleViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CSPLR_ExampleViewPlugin"
                      FILE  "CSPLR_ExampleViewPlugin.json")
    
public:
	CSPLR_ExampleViewPluginFactory(void) {}
	~CSPLR_ExampleViewPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // CSPLR_TEST_VIEW_PLUGIN
