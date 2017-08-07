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
    CSPLR_ExampleViewPlugin() : ViewPlugin("Example View") { }
	~CSPLR_ExampleViewPlugin(void);
    
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
