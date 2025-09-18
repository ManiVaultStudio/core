// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>

#include <QMenu>

/**
 * File menu class
 * 
 * For file-related operations
 * 
 * @author Thomas Kroes
 */
class FileMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    FileMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param event Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

    /**
    * Override to allow app customization
    * Shows the edit... menu item when the F8 key is pressed
    * @param event Pointer to event that occurred
    * @return True if the event was consumed, false to pass it on to the base class
    */
    bool event(QEvent* event) override;

private:
    
    /**
     * Populate the menu
     */
    void populate ();

private:
    mv::gui::TriggerAction    _exitApplictionAction;  /** Trigger action to exit the application */
};
