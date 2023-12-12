// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"

#include <QStandardItemModel>
#include <QMimeData>

namespace mv {

/**
 * Data hierarchy model class
 *
 * Standard item model for managing the data hierarchy
 *
 * @author Thomas Kroes
 */
class DataHierarchyModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Dataset columns */
    enum Column {
        Name,           /** Name of the dataset */
        ID,             /** Globally unique dataset identifier */
        Progress,       /** Task progress in percentage */
        GroupIndex,     /** Dataset group index */
        IsGroup,        /** Whether the dataset is composed of other datasets */
        IsAnalyzing,    /** Whether an analysis is taking place on the dataset */
        IsLocked,       /** Whether the dataset is locked */

        Count
    };

public:

    /** Base standard model item class for dataset */
    class Item : public QStandardItem, public QObject {
    public:
    
        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(Dataset<DatasetImpl> dataset, bool editable = false);
    
        /**
         * Get dataset
         * return Dataset to display item for
         */
        Dataset<DatasetImpl>& getDataset();

        /**
         * Get dataset
         * return Dataset to display item for
         */
        const Dataset<DatasetImpl>& getDataset() const;
    
    private:
        Dataset<DatasetImpl>    _dataset;   /** Pointer to dataset to display item for */
    };

protected:

    /** Standard model item class for interacting with the dataset name */
    class NameItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        NameItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;
    };

    /** Standard model item class for displaying the dataset identifier */
    class IdItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        IdItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    explicit DataHierarchyModel(QObject* parent = nullptr);

    /** Destructor */
    ~DataHierarchyModel();

    /** Get supported drag actions */
    Qt::DropActions supportedDragActions() const override;

    /**
     * Get item flags
     * @param index Model index
     * @return Item flags
     */
    //Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * For the given item indices return the MIME data intended to pass information
     * in the case of e.g. dragging or dropping data.
     */
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

    /**
     * Add a data hierarchy item to the model
     * @param parentModelIndex Model index of the parent data hierarchy item 
     * @param dataHierarchyItem Reference to the data hierarchy item
     */
    //bool addDataHierarchyModelItem(const QModelIndex& parentModelIndex, mv::DataHierarchyItem& dataHierarchyItem);

    /**
     * Remove a data hierarchy item from the model
     * @param parentModelIndex Model index of the parent data hierarchy item
     */
    //bool removeDataHierarchyModelItem(const QModelIndex& modelIndex);
};

}