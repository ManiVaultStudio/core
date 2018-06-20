#pragma once

#include <LoaderPlugin.h>

#include <string>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class MCV_FcsHeader;

class FcsLoader : public LoaderPlugin
{
public:
    FcsLoader();
    ~FcsLoader(void);
    
    void init();

    bool loadData() Q_DECL_OVERRIDE;
private:
    std::string fetchValueForKey(const std::string key);

    std::string _fileName;
    std::string _textHeader;
    char _separator;
};

// =============================================================================
// Factory
// =============================================================================

class FcsLoaderFactory : public LoaderPluginFactory
{
    Q_INTERFACES(hdps::plugin::LoaderPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.FcsLoader"
                      FILE  "FcsLoader.json")
    
public:
    FcsLoaderFactory(void) {}
    ~FcsLoaderFactory(void) {}
    
    LoaderPlugin* produce();
};
