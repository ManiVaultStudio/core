#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// Data
// =============================================================================

class TextData : public RawData
{
public:
    TextData() : RawData("Text") { }
    ~TextData(void) override;
    
    void init() override;

    hdps::Set* createSet() const override;

private:
    std::vector<QString> _data;
};

// =============================================================================
// Factory
// =============================================================================

class TextDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.lumc.TextData"
                      FILE  "TextData.json")
    
public:
    TextDataFactory(void) {}
    ~TextDataFactory(void) override {}
    
    RawData* produce() override;
};
