// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Dataset.h"

#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>

namespace mv
{

/**
 * Datasets model class
 *
 * Standard item model class for datasets
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractDatasetsModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Task columns */
    enum class Column {
        Name,       /** Name of the dataset */
        Location,   /** Location of the dataset */
        ID,         /** Globally unique identifier of the dataset */

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Header standard model item class */
    class HeaderItem : public QStandardItem {
    public:

        /**
         * Construct with \p columHeaderInfo
         * @param columHeaderInfo Column header info
         */
        HeaderItem(const ColumHeaderInfo& columHeaderInfo);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

    private:
        ColumHeaderInfo     _columHeaderInfo;   /** Column header info */
    };

public:

    /** Base standard model item class for a dataset */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with smart pointer to \p dataset
         * @param dataset Smart pointer to the dataset to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(Dataset<DatasetImpl> dataset, bool editable = false);

        /**
         * Get dataset
         * return Reference to smart pointer to dataset to display item for
         */
        Dataset<DatasetImpl>& getDataset();

    private:
        Dataset<DatasetImpl>    _dataset;      /** Smart pointer to the dataset to display item for */
    };

    /** Standard model item class for displaying the dataset name */
    class CORE_EXPORT NameItem final : public Item {
    public:

        /**
         * Construct with smart pointer to \p dataset
         * @param dataset Smart pointer to the dataset to display item for
         */
        NameItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the dataset location */
    class CORE_EXPORT LocationItem final : public Item{
    public:

        /**
         * Construct with smart pointer to \p dataset
         * @param dataset Smart pointer to the dataset to display item for
         */
        LocationItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task identifier */
    class CORE_EXPORT IdItem final : public Item {
    public:

        /** Use base dataset item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Convenience class for combining task items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with smart pointer to \p dataset
         * @param dataset Smart pointer to the dataset to display item for
         */
        Row(Dataset<DatasetImpl> dataset) :
            QList<QStandardItem*>()
        {
            append(new NameItem(dataset));
            append(new LocationItem(dataset));
            append(new IdItem(dataset));
        }
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractDatasetsModel(QObject* parent = nullptr);

    /**
     * Get item from smart pointer to \p dataset
     * @return Pointer to found item, nullptr otherwise
     */
    QStandardItem* itemFromDataset(Dataset<DatasetImpl> dataset) const;

private:

    /**
     * Add \p dataset to the model (this method is called when a dataset is added to the manager)
     * @param dataset Smart pointer to dataset to add
     */
    virtual void addDataset(Dataset<DatasetImpl> dataset) = 0;

    /**
     * Remove \p dataset from the model (this method is called when a dataset is about to be removed from the manager)
     * @param dataset Smart pointer to task to remove
     */
    virtual void removeDataset(Dataset<DatasetImpl> dataset) final;

    friend class Item;
};

}
