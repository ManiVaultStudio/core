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
        colors->setName(getName());
        colors->indices = indices;
        return colors;
    }

    /**
     * Create subset
     * @param subsetName Name of the subset
     * @param parentSetName Name of the parent dataset
     * @param visible Whether the subset will be visible in the UI
     */
    QString createSubset(const QString subsetName = "subset", const QString parentSetName = "",const bool& visible = true) const override
    {
        const hdps::DataSet& selection = getSelection();

        return _core->createSubsetFromSelection(selection, *this, subsetName, parentSetName, visible);
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
