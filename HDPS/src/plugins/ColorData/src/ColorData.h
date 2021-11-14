#pragma once

#include <RawData.h>
#include <Set.h>

#include <QColor>
#include <vector>

// =============================================================================
// Data Type
// =============================================================================

const hdps::DataType ColorType = hdps::DataType(QString("Colors"));

// =============================================================================
// Raw Data
// =============================================================================

class ColorData : public hdps::plugin::RawData
{
public:
    ColorData(const hdps::plugin::PluginFactory* factory) : hdps::plugin::RawData(factory, ColorType) { }
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
        colors->setGuiName(getGuiName());
        colors->indices = indices;
        return colors;
    }

    /**
     * Create subset and attach it to the root of the hierarchy when the parent data set is not specified or below it otherwise
     * @param subsetGuiName Name of the subset in the GUI
     * @param parentDataSet Pointer to parent dataset (if any)
     * @param visible Whether the subset will be visible in the UI
     * @return Reference to the created subset
     */
    DataSet& createSubset(const QString subsetGuiName, DataSet* parentDataSet, const bool& visible = true) const override {
        return _core->createSubsetFromSelection(getSelection(), *this, subsetGuiName, parentDataSet, visible);
    }

    /** Get icon for the dataset */
    QIcon getIcon() const override;

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
    
    /** Returns the plugin icon */
    QIcon getIcon() const override;

    hdps::plugin::RawData* produce() override;
};
