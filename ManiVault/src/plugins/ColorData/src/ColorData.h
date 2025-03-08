// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "colordata_export.h"

#include <RawData.h>
#include <Set.h>

#include <QColor>
#include <vector>

using namespace mv;

const mv::DataType ColorType = mv::DataType(QString("Colors"));

// =============================================================================
// Raw Data
// =============================================================================

class COLORDATA_EXPORT ColorData : public mv::plugin::RawData
{
public:
    ColorData(const mv::plugin::PluginFactory* factory) : mv::plugin::RawData(factory, ColorType) { }
    ~ColorData(void) override;
    
    void init() override;

    uint count();

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    Dataset<DatasetImpl> createDataSet(const QString& guid = "") const override;

private:
    std::vector<QColor> _colors;
};

class COLORDATA_EXPORT Colors : public mv::DatasetImpl
{
public:
    Colors(QString dataName, bool mayUnderive = true, const QString& guid = "") :
        DatasetImpl(dataName, mayUnderive, guid)
    {
    }

    ~Colors() override { }

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    Dataset<DatasetImpl> copy() const override
    {
        auto colors = new Colors(getRawDataName());

        colors->setText(text());
        colors->indices = indices;

        return Dataset<DatasetImpl>(colors);
    }

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const override {
        return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

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

class ColorDataFactory : public mv::plugin::RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.ColorData"
                      FILE  "ColorData.json")
    
public:
    ColorDataFactory();

    ~ColorDataFactory() override {}
    
    /**
     * Get the read me markdown file URL
     * @return URL of the read me markdown file
     */
    QUrl getReadmeMarkdownUrl() const override;

    /**
     * Get the URL of the GitHub repository
     * @return URL of the GitHub repository
     */
    QUrl getRepositoryUrl() const override;

    mv::plugin::RawData* produce() override;
};
