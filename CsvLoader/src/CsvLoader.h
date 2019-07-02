#pragma once

#include <LoaderPlugin.h>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class CsvLoader : public LoaderPlugin
{
public:
    CsvLoader() : LoaderPlugin("CSV Loader") { }
    ~CsvLoader(void) override;
    
    void init() override;

    void loadData() Q_DECL_OVERRIDE;
};


// =============================================================================
// Factory
// =============================================================================

class CsvLoaderFactory : public LoaderPluginFactory
{
    Q_INTERFACES(hdps::plugin::LoaderPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.CsvLoader"
                      FILE  "CsvLoader.json")
    
public:
    CsvLoaderFactory(void) {}
    ~CsvLoaderFactory(void) override {}
    
    LoaderPlugin* produce() override;
};
