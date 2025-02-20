// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "textdata_export.h"

#include "OrderedMap.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace mv;
using namespace mv::plugin;

class InfoAction;

// =============================================================================
// Data Type
// =============================================================================

const mv::DataType TextType = mv::DataType(QString("Text"));

// =============================================================================
// Raw Data
// =============================================================================

class TEXTDATA_EXPORT TextData : public mv::plugin::RawData
{
public:
    TextData(PluginFactory* factory) :
        mv::plugin::RawData(factory, TextType)
    {
    }

    ~TextData(void) override;

    void init() override;

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    Dataset<DatasetImpl> createDataSet(const QString& guid = "") const override;

    /**
     * Determine if a column exists with the given header name
     * @return Whether a column with the given header name exists
     */
    bool hasColumn(QString columnName) const
    {
        return _data.hasColumn(columnName);
    }

    /**
     * Get a column by the name of its header
     * @return The column of text data associated with the given header name
     */
    const std::vector<QString>& getColumn(QString columnName) const
    {
        return _data.getColumn(columnName);
    }

    void addColumn(QString columnName, std::vector<QString>& columnData)
    {
        _data.addColumn(columnName, columnData);
    }

    /**
     * Get the number of elements stored in each column.
     * @return Number of rows per column
     */
    size_t getNumRows() const
    {
        return _data.getNumRows();
    }

    /**
     * Get the number of columns.
     * @return Number of columns
     */
    size_t getNumColumns() const
    {
        return _data.getNumColumns();
    }

    /**
     * Get an ordered list of names associated to each column. Names are listed in the order their columns were added.
     * @return Ordered list of column header names
     */
    const std::vector<QString>& getColumnNames() const
    {
        return _data.getColumnNames();
    }

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save widget action to variant
        * @return Variant representation of the widget action
        */
    QVariantMap toVariantMap() const override;

private:
    OrderedMap _data;
};

class TEXTDATA_EXPORT Text : public DatasetImpl
{
public:
    Text(QString dataName, bool mayUnderive = true, const QString& guid = "") :
        DatasetImpl(dataName, mayUnderive, guid),
        _infoAction(nullptr)
    {
    }

    ~Text() override { }

    void init() override;

    Dataset<DatasetImpl> copy() const override
    {
        auto text = new Text(getRawDataName());

        text->setText(this->text());
        text->indices = indices;

        return text;
    }

    /**
     * Determine if a column exists with the given header name
     * @return Whether a column with the given header name exists
     */
    bool hasColumn(QString columnName) const
    {
        return getRawData<TextData>()->hasColumn(columnName);
    }

    /**
     * Get a column by the name of its header
     * @return The column of text data associated with the given header name
     */
    const std::vector<QString>& getColumn(QString columnName) const
    {
        return getRawData<TextData>()->getColumn(columnName);
    }

    /**
     * Add a column with a given header name to the dataset
     */
    void addColumn(QString columnName, std::vector<QString>& columnData)
    {
        getRawData<TextData>()->addColumn(columnName, columnData);
    }

    size_t getNumRows() const
    {
        return getRawData<TextData>()->getNumRows();
    }

    size_t getNumColumns() const
    {
        return getRawData<TextData>()->getNumColumns();
    }

    const std::vector<QString>& getColumnNames() const
    {
        return getRawData<TextData>()->getColumnNames();
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
        return mv::data().createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
        * Save widget action to variant
        * @return Variant representation of the widget action
        */
    QVariantMap toVariantMap() const override;

public: // Action getters
    InfoAction& getInfoAction();

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

    /** De-select all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;

    InfoAction* _infoAction;                    /** Non-owning pointer to info action */

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class TextDataFactory : public RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.TextData"
                      FILE  "TextData.json")
    
public:
    TextDataFactory();

    ~TextDataFactory() override {}

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
