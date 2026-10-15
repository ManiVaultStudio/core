// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"
#include "Dataset.h"
#include "Set.h"

#include "actions/StringAction.h"

#include <QList>
#include <QStandardItem>

namespace mv {

/**
 * Selections model class
 *
 * Standard item model for listing selections
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT SelectionsModel final : public StandardItemModel
{
public:

    /** Selection columns */
    enum Column {
        Name,           /** Name of the selection dataset */
        ID,             /** Globally unique identifier of the selection dataset */
        RawDataName,    /** Name of the raw dataset on which the selection set is based */
        RawDataType,    /** Type of the raw dataset on which the selection set is based */

        Count
    };

private:

    /** Base standard model item class for selection datasets */
    class Item : public QStandardItem {
    public:

        /**
         * Construct with \p selection
         * @param selection Selection dataset
         */
        Item(Dataset<DatasetImpl> selection);

        /**
         * Get selection
         * return Selection dataset
         */
        Dataset<DatasetImpl> getSelection() const;

    private:
        Dataset<DatasetImpl> _selection;   /** Selection dataset */
    };

    /** Standard model item class for selection dataset name */
    class NameItem : public Item {
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
                    return "Name";

                case Qt::ToolTipRole:
                    return "The name of the selection dataset";
            }

            return {};
        }
    };

    /** Standard model item class for selection dataset identifier */
    class IdItem : public Item {
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
                    return "ID";

                case Qt::ToolTipRole:
                    return "Globally unique identifier of the selection dataset";
            }

            return {};
        }
    };

    /** Standard model item class for selection dataset raw data name */
    class RawDataNameItem : public Item {
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
                    return "Raw data name";

                case Qt::ToolTipRole:
                    return "Name of the raw dataset on which the selection set is based";
            }

            return {};
        }
    };

    /** Standard model item class for selection dataset raw data type */
    class RawDataTypeItem : public Item {
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
                    return "Raw data type";

                case Qt::ToolTipRole:
                    return "Type of the raw dataset on which the selection set is based";
            }

            return {};
        }
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit SelectionsModel(QObject* parent = nullptr);

    /**
     * Get header data for \p section, \p orientation and display \p role
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

protected:

    /** Convenience class for combining selection items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p selection
         * @param selection Pointer to selection dataset to display item for
         */
        Row(Dataset<DatasetImpl> selection) : QList<QStandardItem*>()
        {
            append(new NameItem(selection));
            append(new IdItem(selection));
            append(new RawDataNameItem(selection));
            append(new RawDataTypeItem(selection));
        }

    };

protected: // Drag-and-drop behavior

    /** Get supported drag actions */
    Qt::DropActions supportedDragActions() const override {
        return Qt::IgnoreAction;
    };

    /** Get the supported drop actions */
    Qt::DropActions supportedDropActions() const override {
        return Qt::IgnoreAction;
    };

public:

    /** Populate the model with selections from the data manager */
    void populateFromDataManager();
};

}