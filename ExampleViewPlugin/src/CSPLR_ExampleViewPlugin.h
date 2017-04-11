#ifndef CSPLR_TEST_VIEW_PLUGIN
#define CSPLR_TEST_VIEW_PLUGIN

#include <MCV_ViewPlugin.h>

// =============================================================================
// View
// =============================================================================

class CSPLR_ExampleViewPlugin : public MCV_ViewPlugin
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

class CSPLR_ExampleViewPluginFactory : public MCV_ViewPluginFactory
{
    Q_INTERFACES(MCV_ViewPluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CSPLR_ExampleViewPlugin"
                      FILE  "CSPLR_ExampleViewPlugin.json")
    
public:
	CSPLR_ExampleViewPluginFactory(void) {}
	~CSPLR_ExampleViewPluginFactory(void) {}
    
    QSharedPointer<MCV_ViewPlugin> produce();
};

#endif // CSPLR_TEST_VIEW_PLUGIN
