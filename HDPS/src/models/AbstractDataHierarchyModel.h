// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "Task.h"
#include "Set.h"

#include "actions/TaskAction.h"

#include <QStandardItemModel>
#include <QMimeData>

namespace mv {

/**
 * Abstract data hierarchy model class
 *
 * Abstract standard item model class for managing the data hierarchy
 *
 * @author Thomas Kroes
 */
class AbstractDataHierarchyModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Dataset columns */
    enum Column {
        Name,               /** Name of the dataset */
        Location,           /** Location of the dataset */
        DatasetId,          /** Globally unique dataset identifier */
        SourceDatasetId,    /** Globally unique dataset identifier of the source dataset (if this dataset is derived) */
        RawDataId,          /** Globally unique identifier of the associated raw data */
        Progress,           /** Task progress in percentage */
        GroupIndex,         /** Dataset group index */
        IsVisible,          /** Whether the dataset is visible or not */
        IsGroup,            /** Whether the dataset is composed of other datasets */
        IsLocked,           /** Whether the dataset is locked */
        IsDerived,          /** Whether the dataset is derived from another dataset */

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
        Item(Dataset<DatasetImpl> DataHierarchyItem, bool editable = false);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

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

        /** Refreshes the data display */
        void refreshData() {
            emitDataChanged();
        }

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
    };

    /** Standard model item class for showing the dataset location */
    class LocationItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        LocationItem(Dataset<DatasetImpl> dataset);

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
                    return "Location";

                case Qt::ToolTipRole:
                    return "Location of the dataset";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the dataset identifier */
    class DatasetIdItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        DatasetIdItem(Dataset<DatasetImpl> dataset);

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

    /** Standard model item class for displaying the source dataset identifier */
    class SourceDatasetIdItem final : public Item {
    public:

        /** Use base item constructor */
        using Item::Item;

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
                    return "Source Dataset ID";

                case Qt::ToolTipRole:
                    return "The globally unique identifier of the source dataset";
            }

            return {};
        }
    };

    /** Standard model item class for displaying the raw data identifier */
    class RawDataIdItem final : public Item {
    public:

        /** Use base item constructor */
        using Item::Item;

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
                    return "Raw Data ID";

                case Qt::ToolTipRole:
                    return "The globally unique identifier of the raw data";
            }

            return {};
        }
    };

public:

    /** Standard model item class for displaying the dataset task progress */
    class ProgressItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        ProgressItem(Dataset<DatasetImpl> dataset);

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
                    return "Task progress";

                case Qt::ToolTipRole:
                    return "The dataset task progress";
            }

            return {};
        }

        /**
         * Get dataset task
         * @return Reference to dataset task
         */
        Task& getDatasetTask() {
            return getDataset()->getTask();
        }

        /**
         * Get task action
         * @return Task action for use in item delegate (its built-in progress action)
         */
        gui::TaskAction& getTaskAction() {
            return _taskAction;
        }

        /**
         * Create delegate editor widget as child of \p parent
         * @param parent Pointer to delegate parent
         * @return Pointer to created delegate editor widget
         */
        QWidget* createDelegateEditorWidget(QWidget* parent);

    private:
        gui::TaskAction     _taskAction;    /** Task action for use in item delegate (uses its built-in progress action) */
    };

protected:

    /** Standard model item class for displaying the dataset group index */
    class GroupIndexItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        GroupIndexItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;

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
                    return "Group index";

                case Qt::ToolTipRole:
                    return "The dataset group index";
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the dataset is visible or not */
    class IsVisibleItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        IsVisibleItem(Dataset<DatasetImpl> dataset);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;

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
                    return "Is visible";

                case Qt::ToolTipRole:
                    return "Whether the dataset is visible or not";
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the dataset belongs to a group */
    class IsGroupItem final : public Item {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        IsGroupItem(Dataset<DatasetImpl> dataset);

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
                    return "Is group";

                case Qt::ToolTipRole:
                    return "Whether the dataset belongs to a group";
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the dataset is locked */
    class IsLockedItem final : public Item {
    public:

        /** Use base item constructor */
        using Item::Item;

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
                    return "Is locked";

                case Qt::ToolTipRole:
                    return "Whether the dataset is locked";
            }

            return {};
        }
    };

    /** Standard model item class for displaying whether the dataset is derived from another dataset */
    class IsDerivedItem final : public Item {
    public:

        /** Use base item constructor */
        using Item::Item;

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
                    return "Is derived";

                case Qt::ToolTipRole:
                    return "Whether the dataset is derived from another dataset";
            }

            return {};
        }
    };

    /** Convenience class for combining dataset items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        Row(Dataset<DatasetImpl> dataset);
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit AbstractDataHierarchyModel(QObject* parent = nullptr);

    /** Get supported drag actions */
    Qt::DropActions supportedDragActions() const override;

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    /**
     * Get MIME data for supplied \p indexes
     * @param indexes Index list to get the MIME data for
     * @return Pointer to MIME data
     */
    QMimeData* mimeData(const QModelIndexList& indexes) const override;

    /**
     * Get item of \p ItemType by \p modelIndex
     * @param modelIndex Item model index
     * @return Pointer to item
     */
    template<typename ItemType = Item>
    ItemType* getItem(const QModelIndex& modelIndex) {
        return dynamic_cast<ItemType*>(itemFromIndex(modelIndex));
    }

    /**
     * Hides item with \p index and its descendants
     * @param index Index of the item to hide (column index must be zero)
     */
    void hideItem(const QModelIndex& index);

    /**
     * Un-hides item with \p index
     * @param index Index of the item to un-hide (column index must be zero)
     */
    void unhideItem(const QModelIndex& index);
};

}