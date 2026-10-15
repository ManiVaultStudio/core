// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "TriggerAction.h"

class QItemSelectionModel;
class QAbstractItemModel;

namespace mv::gui {

/**
 * Model selection action class
 *
 * Action for selecting items in a model
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT ModelSelectionAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object, action \p title and possibly pointer to an item \p selectionModel
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param selectionModel Pointer to item selection model
     */
    Q_INVOKABLE explicit ModelSelectionAction(QObject* parent, const QString& title, QItemSelectionModel* selectionModel = nullptr);

    /**
     * Initializes with a pointer to the \p selectionModel
     * @param selectionModel Pointer to item selection model
     */
    void initialize(QItemSelectionModel* selectionModel);

public: // Action getters

    TriggerAction& getSelectAllAction() { return _selectAllAction; }
    TriggerAction& getClearSelectionAction() { return _clearSelectionAction; }
    TriggerAction& getInvertSelectionAction() { return _invertSelectionAction; }

private:
    QItemSelectionModel*    _selectionModel;            /** Pointer to item selection model */
    QAbstractItemModel*     _sourceModel;               /** Pointer to selection model source model */
    TriggerAction           _selectAllAction;           /** Select all action */
    TriggerAction           _clearSelectionAction;      /** Clear selection action */
    TriggerAction           _invertSelectionAction;     /** Invert selection action */
};

}   

Q_DECLARE_METATYPE(mv::gui::ModelSelectionAction)

inline const auto modelSelectionActionMetaTypeId = qRegisterMetaType<mv::gui::ModelSelectionAction*>("mv::gui::ModelSelectionAction");
