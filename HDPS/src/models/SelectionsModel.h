// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/StringAction.h"

#include <QStandardItemModel>

namespace mv {

/**
 * Selections model class
 *
 * Standard item model for listing selections
 *
 * @author Thomas Kroes
 */
class SelectionsModel final : public QStandardItemModel
{
public:

    /** Selection columns */
    enum Column {
        Name,       /** Name of the selection dataset */

        Count
    };

private:

    /** Base standard model item class for selection datasets */
    class SelectionItem : public QStandardItem {
    public:

        /**
         * Construct with \p selection
         * @param selection Selection dataset
         */
        SelectionItem(Dataset<DatasetImpl> selection);

        /**
         * Get selection
         * return Selection dataset
         */
        Dataset<DatasetImpl> getSelection() const;

    private:
        Dataset<DatasetImpl> _selection;   /** Selection dataset */
    };

    /** Standard model item class for selection dataset name */
    class SelectionNameItem : public SelectionItem {
    public:

        /** Use base item constructor */
        using SelectionItem::SelectionItem;

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
        Row(Dataset<DatasetImpl> selection);
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

public: // Action getters

    gui::StringAction& getCountAction() { return _countAction; }

private:
    gui::StringAction   _countAction;    /** String action for displaying the number of selections */
};

}