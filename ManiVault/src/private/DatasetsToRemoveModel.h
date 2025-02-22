// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Dataset.h>
#include <Set.h>

#include <QList>
#include <QStandardItem>
#include <QStandardItemModel>

class DataHierarchyItem;

/**
 * Datasets to remove model class
 *
 * Standard item model for selecting datasets to remove from a model
 *
 * @author Thomas Kroes
 */
class DatasetsToRemoveModel final : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Dataset columns */
    enum Column {
        Name,           /** Name of the dataset */
        DatasetId,      /** Globally unique dataset identifier */
        Info,           /** Item info */

        Count
    };

public:

    /** Base standard model item class for dataset */
    class Item : public QStandardItem, public QObject {
    public:
    
        /**
         * Construct with pointer to \p dataset, reference to owning \p datasetsToRemoveModel and \p editable toggle
         * @param dataset Pointer to dataset to display item for
         * @param datasetsToRemoveModel Datasets to remove model
         * @param editable Whether the model item is editable or not
         */
        Item(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel, bool editable = false);

        /** Remove row from Row#allRows when destructed */
        ~Item();

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Performs initialization after the item has been added to the model */
        virtual void initialize();

    protected:

        /**
         * Get datasets remove model
         * return Reference to dataset remove model
         */
        DatasetsToRemoveModel& getDatasetRemoveModel();

        /**
         * Get dataset
         * return Dataset to display item for
         */
        mv::Dataset<mv::DatasetImpl>& getDataset();

        /**
         * Get dataset
         * return Dataset to display item for
         */
        const mv::Dataset<mv::DatasetImpl>& getDataset() const;

        /** Refreshes the data display */
        void refreshData() {
            emitDataChanged();
        }

    private:

        /** Update item read-only state */
        void updateReadOnly();

    private:
        DatasetsToRemoveModel&          _datasetsToRemoveModel;     /** * Reference to owning datasets to remove model */
        mv::Dataset<mv::DatasetImpl>    _dataset;                   /** Pointer to dataset to display item for */

    protected:
        friend class DatasetsToRemoveModel;
    };

private:

    /** Standard model item class for interacting with the dataset name */
    class NameItem final : public Item {
    public:

        /**
         * Construct with pointer to \p dataset and reference to owning \p datasetsToRemoveModel
         * @param dataset Pointer to dataset to display item for
         * @param datasetsToRemoveModel Reference to owning \p datasetsToRemoveModel
         */
        NameItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Name";

                case Qt::ToolTipRole:
                    return "Name of the dataset";
            }

            return {};
        }

        /**
         * Set keep descendants to \p keepDescendants
         * @param keepDescendants Whether to keep descendants or not
         */
        void setKeepDescendants(bool keepDescendants);

        /** Updates the check state based on its parent item */
        void updateCheckState();
    };

    /** Standard model item class for displaying the dataset identifier */
    class DatasetIdItem final : public Item {
    public:

        /**
         * Construct with pointer to \p dataset and reference to owning \p datasetsToRemoveModel
         * @param dataset Pointer to dataset to display item for
         * @param datasetsToRemoveModel Reference to owning \p datasetsToRemoveModel
         */
        DatasetIdItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                case Qt::EditRole:
                    return "Dataset ID";

                case Qt::ToolTipRole:
                    return "The globally unique identifier of the dataset";
            }

            return {};
        }
    };

    /** Standard model item class for storing the item info */
    class InfoItem final : public Item {
    public:

        /**
         * Construct with pointer to \p dataset and reference to owning \p datasetsToRemoveModel
         * @param dataset Pointer to dataset to display item for
         * @param datasetsToRemoveModel Reference to owning \p datasetsToRemoveModel
         */
        InfoItem(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get header data for \p orientation and \p role
         * @param orientation Horizontal/vertical
         * @param role Data role
         * @return Header data
         */
        static QVariant headerData(Qt::Orientation orientation, int role) {
            switch (role) {
                case Qt::DisplayRole:
                    return "";

                case Qt::EditRole:
                    return "Info";

                case Qt::ToolTipRole:
                    return "Additional info about the dataset in the context of removal";
            }

            return {};
        }


    private:
        bool    _visible;       /** Whether the item is visible or not */
    };

protected:

    /** Convenience class for combining dataset items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with pointer to \p dataset and reference to owning \p datasetsToRemoveModel
         * @param dataset Pointer to dataset to display item for
         * @param datasetsToRemoveModel Reference to owning \p datasetsToRemoveModel
         */
        Row(mv::Dataset<mv::DatasetImpl> dataset, DatasetsToRemoveModel& datasetsToRemoveModel);

        /**
         * Get name item
         * @return Pointer to name item
         */
        NameItem* getNameItem();

        /** Update read-only status for all items */
        void updateReadOnly();

        /**
         * Set keep descendants to \p keepDescendants for all name items
         * @param keepDescendants Whether to keep descendants or not
         */
        void setKeepDescendants(bool keepDescendants);

    protected:
        static std::vector<Row> allRows;

        friend class DatasetsToRemoveModel;
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit DatasetsToRemoveModel(QObject* parent = nullptr);

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Set datasets to \p datasets and build the model
     * @param datasets Datasets
     */
    void setDatasets(mv::Datasets datasets);

    /**
     * Get datasets to remove (these are the datasets checked/selected in the selection tree)
     * @return Datasets to remove (bottom-up)
     */
    mv::Datasets getDatasetsToRemove() const;

    /**
     * Get item of \p ItemType by \p modelIndex
     * @param modelIndex Item model index
     * @return Pointer to item
     */
    template<typename ItemType = Item>
    ItemType* getItem(const QModelIndex& modelIndex) {
        return dynamic_cast<ItemType*>(itemFromIndex(modelIndex));
    }

    /** Check all items */
    void checkAll();

private:

    /**
     * Add \p dataset to the model
     * @param dataset Smart pointer to the data set to add
     * @param selectedDatasets Selected datasets
     */
    void addDataset(mv::Dataset<mv::DatasetImpl> dataset, mv::Datasets selectedDatasets);

    /** Updates the read-only state of all name items */
    void updateReadOnlyForAllNameItems();

    /**
     * Set keep descendants to \p keepDescendants for all name items
     * @param keepDescendants Whether to keep descendants or not
     */
    void setKeepDescendantsForAllNameItems();

    /** Updates the check states for all name items */
    void updateCheckStatesForAllNameItems();
};
