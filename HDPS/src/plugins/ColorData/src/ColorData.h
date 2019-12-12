#pragma once

#include <RawData.h>
#include <Set.h>

#include <QColor>
#include <vector>

using namespace hdps;
using namespace hdps::plugin;

// =============================================================================
// Data
// =============================================================================

class ColorData : public hdps::RawData
{
public:
    ColorData() : hdps::RawData("Colors", ColorType) { }
    ~ColorData(void) override;
    
    void init() override;

    uint count();

    hdps::DataSet* createDataSet() const override;

private:
    std::vector<QColor> _colors;
};

// =============================================================================
// Color Data Set
// =============================================================================

class Colors : public hdps::DataSet
{
public:
    Colors(hdps::CoreInterface* core, QString dataName) : DataSet(core, dataName) { }
    ~Colors() override { }

    DataSet* copy() const override
    {
        Colors* colors = new Colors(_core, getDataName());
        colors->setName(getName());
        colors->indices = indices;
        return colors;
    }

    void createSubset() const override
    {
        const hdps::DataSet& selection = _core->requestSelection(getDataName());

        _core->createSubsetFromSelection(selection, getDataName(), "Subset");
    }

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class ColorDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.lumc.ColorData"
                      FILE  "ColorData.json")
    
public:
    ColorDataFactory(void) {}
    ~ColorDataFactory(void) override {}
    
    hdps::RawData* produce() override;
};
