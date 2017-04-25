#ifndef CSPLR_TEST_DATA_PLUGIN
#define CSPLR_TEST_DATA_PLUGIN

#include <DataTypePlugin.h>
#include <QImage>

using namespace hdps::plugin;

// =============================================================================
// Data
// =============================================================================

class CSPLR_ExampleDataPlugin : public DataTypePlugin
{
public:
    CSPLR_ExampleDataPlugin() : DataTypePlugin("Example Data") { }
    ~CSPLR_ExampleDataPlugin(void);
    
    void init();

    void setData(QImage image);
private:
    std::vector<QColor> data;
};


// =============================================================================
// Factory
// =============================================================================

class CSPLR_ExampleDataPluginFactory : public DataTypePluginFactory
{
    Q_INTERFACES(hdps::plugin::DataTypePluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CSPLR_ExampleDataPlugin"
                      FILE  "CSPLR_ExampleDataPlugin.json")
    
public:
	CSPLR_ExampleDataPluginFactory(void) {}
	~CSPLR_ExampleDataPluginFactory(void) {}
    
    DataTypePlugin* produce();
};

#endif // CSPLR_TEST_DATA_PLUGIN
