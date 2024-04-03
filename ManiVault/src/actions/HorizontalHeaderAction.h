// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "ToggleAction.h"

class QHeaderView;

namespace mv::gui {


// Forward declare to avoid circular dependency
class OptionsAction;

/**
 * Horizontal header action class
 *
 * Action for configuring a horizontal header
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT HorizontalHeaderAction final : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object, action \p title and possibly pointer to an item \p selectionModel
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param horizontalHeaderView Pointer to horizontal header view
     */
    Q_INVOKABLE explicit HorizontalHeaderAction(QObject* parent, const QString& title, QHeaderView* horizontalHeaderView = nullptr);

    /**
     * Initializes with a pointer to the \p horizontalHeaderView
     * @param horizontalHeaderView Pointer to horizontal header view
     */
    void initialize(QHeaderView* horizontalHeaderView);

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Re-populate the columns option action */
    void updateColumnsOptionsAction();

public: // Action getters

    OptionsAction& getColumnsAction() { return *_columnsAction; }

private:
    QHeaderView*    _horizontalHeaderView;  /** Pointer to horizontal header view */
    OptionsAction*  _columnsAction;         /** Column toggling action */
    ToggleAction    _showHeaderAction;            /** Action for toggling the header */
};

}

Q_DECLARE_METATYPE(mv::gui::HorizontalHeaderAction)

inline const auto horizontalHeaderActionMetaTypeId = qRegisterMetaType<mv::gui::HorizontalHeaderAction*>("mv::gui::HorizontalHeaderAction");
