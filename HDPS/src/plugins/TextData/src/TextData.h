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
        text->setName(getName());
        text->indices = indices;
        return text;
    }

    /**
     * Create subset
     * @param subsetName Name of the subset
     * @param parentSetName Name of the parent dataset
     * @param visible Whether the subset will be visible in the UI
     */
    QString createSubset(const QString subsetName = "subset", const QString parentSetName = "", const bool& visible = true) const override
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
