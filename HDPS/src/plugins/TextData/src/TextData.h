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

class TextData : public RawData
{
public:
    TextData() : RawData("Text", TextType) { }
    ~TextData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

private:
    std::vector<QString> _data;
};

// =============================================================================
// Text Data Set
// =============================================================================

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

    QString createSubset(const bool& visibleInGui = true) const override
    {
        const hdps::DataSet& selection = getSelection();

        return _core->createSubsetFromSelection(selection, *this, "Subset", visibleInGui);
    }

    std::vector<unsigned int> indices;
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
