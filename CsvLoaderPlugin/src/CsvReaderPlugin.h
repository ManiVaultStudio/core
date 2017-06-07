#ifndef CSV_READER_PLUGIN_H
#define CSV_READER_PLUGIN_H

#include <LoaderPlugin.h>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class CsvReaderPlugin : public LoaderPlugin
{
public:
    CsvReaderPlugin() : LoaderPlugin("CsvLoader") { }
    ~CsvReaderPlugin(void);
    
    void init();

    void loadData() Q_DECL_OVERRIDE;
};


// =============================================================================
// Factory
// =============================================================================

class CsvReaderPluginFactory : public LoaderPluginFactory
{
    Q_INTERFACES(hdps::plugin::LoaderPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CsvReaderPlugin"
                      FILE  "CsvReaderPlugin.json")
    
public:
    CsvReaderPluginFactory(void) {}
    ~CsvReaderPluginFactory(void) {}
    
    LoaderPlugin* produce();
};

#endif // CSV_READER_PLUGIN_H
