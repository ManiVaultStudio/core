// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/ToggleAction.h>

#include <QMenu>
#include <QPointer>

/**
 * Loaded views menu class
 * 
 * Menu class for toggling loaded view plugins visibility
 * 
 * @author Thomas Kroes
 */
class LoadedViewsMenu : public QMenu
{
protected:
    using ToggleActions = QVector<QPointer<mv::gui::ToggleAction>>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    LoadedViewsMenu(QWidget *parent = nullptr);

    /**
     * Populate the menu
     */
    void populate();

private:

    /**
     * Get actions for loaded views
     * @param systemView Whether to include system views or normal views
     * @return Vector of smart pointers to actions
     */
    ToggleActions getLoadedViewsActions(bool systemView);

private:
    QSharedPointer<QMenu>   _loadedSystemViewsMenu;         /** Shared pointer to menu */
    ToggleActions           _viewsToggleActions;     /** Keep track of allocated view toggle actions and remove the actions upon re-population of the menu */

};
