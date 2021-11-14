#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace hdps;
using namespace hdps::plugin;

// =============================================================================
// Data Type
// =============================================================================

const hdps::DataType TextType = hdps::DataType(QString("Text"));

// =============================================================================
// Raw Data
// =============================================================================

class TextData : public hdps::plugin::RawData
{
public:
    TextData(PluginFactory* factory) : hdps::plugin::RawData(factory, TextType) { }
    ~TextData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

private:
    std::vector<QString> _data;
};

class Text : public DataSet
{
public:
    Text(hdps::CoreInterface* core, QString dataName) : DataSet(core, dataName) { }
    ~Text() override { }

    DataSet* copy() const override
    {
        Text* text = new Text(_core, getDataName());
        text->setGuiName(getGuiName());
        text->indices = indices;
        return text;
    }

    /**
     * Create subset and attach it to the root of the hierarchy when the parent data set is not specified or below it otherwise
     * @param subsetGuiName Name of the subset in the GUI
     * @param parentDataSet Pointer to parent dataset (if any)
     * @param visible Whether the subset will be visible in the UI
     * @return Reference to the created subset
     */
    DataSet& createSubset(const QString subsetGuiName, DataSet* parentDataSet, const bool& visible = true) const override
    {
        return _core->createSubsetFromSelection(getSelection(), *this, subsetGuiName, parentDataSet, visible);
    }

    /** Get icon for the dataset */
    QIcon getIcon() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class TextDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "hdps.TextData"
                      FILE  "TextData.json")
    
public:
    TextDataFactory(void) {}
    ~TextDataFactory(void) override {}

    /** Returns the plugin icon */
    QIcon getIcon() const override;

    hdps::plugin::RawData* produce() override;
};
