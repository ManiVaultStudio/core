#pragma once

#include <RawData.h>
#include <Set.h>

#include <QColor>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// Data
// =============================================================================

class ColorData : public RawData
{
public:
    ColorData() : RawData("Color") { }
    ~ColorData(void) override;
    
    void init() override;

    uint count();

    hdps::Set* createSet() const override;

private:
    std::vector<QColor> _colors;
};

// =============================================================================
// Factory
// =============================================================================

class ColorDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.lumc.ColorData"
                      FILE  "ColorData.json")
    
public:
    ColorDataFactory(void) {}
    ~ColorDataFactory(void) override {}
    
    RawData* produce() override;
};
