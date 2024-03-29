// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "OptionsAction.h"

class QAbstractItemModel;
class QHeaderView;

namespace mv::gui {

/**
 * Model columns action class
 *
 * Action for changing model columns visibility
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT ModelColumnsAction final : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object, action \p title and possibly pointer to an item \p selectionModel
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param model Pointer to item model
     */
    Q_INVOKABLE explicit ModelColumnsAction(QObject* parent, const QString& title, QAbstractItemModel* model = nullptr, QHeaderView* horizontalHeaderView = nullptr);

    /**
     * Initializes with a pointer to the item \p model and \p view
     * @param model Pointer to item model
     * @param horizontalHeaderView Pointer to horizontal header view
     */
    void initialize(QAbstractItemModel* model, QHeaderView* horizontalHeaderView);

private:

    /** Re-populate the columns option action */
    void updateColumnsOptionsAction();

public: // Action getters

    OptionsAction& getColumnsAction() { return _columnsAction; }

private:
    QAbstractItemModel*     _model;                 /** Pointer to item model */
    QHeaderView*            _horizontalHeaderView;  /** Pointer to horizontal header view */
    OptionsAction           _columnsAction;         /** Column toggling action */
};

}

Q_DECLARE_METATYPE(mv::gui::ModelColumnsAction)

inline const auto modelColumnsActionMetaTypeId = qRegisterMetaType<mv::gui::ModelColumnsAction*>("mv::gui::ModelColumnsAction");
