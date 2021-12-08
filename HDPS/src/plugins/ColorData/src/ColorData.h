#pragma once

#include <RawData.h>
#include <Set.h>

#include <QColor>
#include <vector>

using namespace hdps;

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

    /** Create dataset for for raw data */
    Dataset<DatasetImpl> createDataSet() const override;

private:
    std::vector<QColor> _colors;
};

class Colors : public hdps::DatasetImpl
{
public:
    Colors(CoreInterface* core, QString dataName) : DatasetImpl(core, dataName) { }
    ~Colors() override { }

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    Dataset<DatasetImpl> copy() const override
    {
        auto colors = new Colors(_core, getRawDataName());
        colors->setGuiName(getGuiName());
        colors->indices = indices;

        return Dataset<DatasetImpl>(colors);
    }

    /**
     * Create subset and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubset(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const override {
        return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
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
