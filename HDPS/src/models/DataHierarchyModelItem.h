// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QObject>
#include <QVector>

class QMenu;

namespace mv {
    class DataHierarchyItem;
}

class DataHierarchyModelItem : public QObject
{
Q_OBJECT

public:
    using PluginHierarchyItems = QVector<DataHierarchyModelItem*>;

    /** Columns */
    enum Column {
        Name,           /** Name of the dataset */
        GUID,           /** Globally unique dataset identifier */
        Info,           /** Info (e.g. to report progress) */
        Progress,       /** Task progress in percentage */
        GroupIndex,     /** Dataset group index */
        IsGroup,        /** Whether the dataset is composed of other datasets */
        IsAnalyzing,    /** Whether an analysis is taking place on the dataset */
        IsLocked,       /** Whether the dataset is locked */

        _start = Name,
        _end = IsLocked
    };

public:

    /**
     * Constructor
     * @param dataHierarchyItem Pointer to data hierarchy item
     * @param parent Parent (if any)
     */
    explicit DataHierarchyModelItem(mv::DataHierarchyItem* dataHierarchyItem, DataHierarchyModelItem* parent = nullptr);

    /** Destructor */
    virtual ~DataHierarchyModelItem();

    /**
     * Add a child item
     * @param item Child item to add
     */
    void addChild(DataHierarchyModelItem* item);
    
    /** Get/set parent item */
    DataHierarchyModelItem* getParent();
    void setParent(DataHierarchyModelItem* parent);

    /**
     * Get child at row
     * @param row Row index of the child
     * @return Child item
     */
    DataHierarchyModelItem* getChild(const std::int32_t& row);

    /** Returns the child index w.r.t. the parent */
    std::int32_t row() const;

    /** Returns the number of children */
    std::int32_t getNumChildren() const;

    /** Returns the number of columns */
    std::int32_t getNumColumns() const;

    /** Returns the serialized item string */
    //QString serialize() const;

    /**
     * Get data at column
     * @param column Column index
     * @param role Data role
     * @return Data in string format
     */
    QVariant getDataAtColumn(const std::uint32_t& column, int role = Qt::DisplayRole) const;

    /**
     * Renames the dataset to \p datasetName
     * @param datasetName New name of the dataset
     */
    void renameDataset(const QString& datasetName);

    /**
     * Set group index
     * @param groupIndex group index
     */
    void setGroupIndex(const std::int32_t& groupIndex);

    /**
     * Get corresponding data hierarchy item
     * @return Pointer to data hierarchy item
     */
    mv::DataHierarchyItem* getDataHierarchyItem();

    /** Get whether the item is visible or not */
    bool isVisible() const;

protected:

    /**
     * Removes a child (name reference to data hierarchy item)
     * @param dataHierarchyModelItem Pointer to data hierarchy model item
     */
    void removeChild(DataHierarchyModelItem* dataHierarchyModelItem);

protected:
    DataHierarchyModelItem*     _parent;                /** Pointer to parent item */
    PluginHierarchyItems        _children;              /** Pointers to child items */
    mv::DataHierarchyItem*    _dataHierarchyItem;     /** Pointer to data hierarchy item*/
};
