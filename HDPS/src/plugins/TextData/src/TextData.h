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

    Dataset<DatasetImpl> createDataSet() const override;

private:
    std::vector<QString> _data;
};

class Text : public DatasetImpl
{
public:
    Text(hdps::CoreInterface* core, QString dataName) : DatasetImpl(core, dataName) { }
    ~Text() override { }

    Dataset<DatasetImpl> copy() const override
    {
        auto text = new Text(_core, getRawDataName());
        text->setGuiName(getGuiName());
        text->indices = indices;
        
        return text;
    }

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const override
    {
        return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

    /** Get icon for the dataset */
    QIcon getIcon() const override;

public: // Selection

    /**
     * Get selection
     * @return Selection indices
     */
    std::vector<std::uint32_t>& getSelectionIndices() override;

    /**
     * Select by indices
     * @param indices Selection indices
     */
    void setSelectionIndices(const std::vector<std::uint32_t>& indices) override;

    /** Determines whether items can be selected */
    bool canSelect() const override;

    /** Determines whether all items can be selected */
    bool canSelectAll() const override;

    /** Determines whether there are any items which can be deselected */
    bool canSelectNone() const override;

    /** Determines whether the item selection can be inverted (more than one) */
    bool canSelectInvert() const override;

    /** Select all items */
    void selectAll() override;

    /** Deselect all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;


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
