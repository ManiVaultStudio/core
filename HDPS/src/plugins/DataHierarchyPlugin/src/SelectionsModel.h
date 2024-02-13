// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <Dataset.h>
#include <Set.h>

#include <actions/StringAction.h>

#include <QStandardItemModel>

/**
 * Selections model class
 *
 * Standard item model for storing selections
 *
 * @author Thomas Kroes
 */
class SelectionsModel final : public QStandardItemModel
{
public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit SelectionsModel(QObject* parent = nullptr);

protected:

    /** Convenience class for combining dataset items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct with \p dataset
         * @param dataset Pointer to dataset to display item for
         */
        Row(mv::Dataset<mv::DatasetImpl> dataset);
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

    /** Populate the model with datasets from the data manager */
    void populateFromDataManager();

public: // Action getters

    mv::gui::StringAction& getCountAction() { return _countAction; }

private:
    mv::gui::StringAction   _countAction;    /** String action for displaying the number of selections */
};
