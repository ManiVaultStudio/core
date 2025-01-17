// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>

#include <QMenu>

namespace mv::gui {
    class TriggerAction;
}

/**
 * Help menu class
 * 
 * Menu class for help functionality
 * 
 * @author Thomas Kroes
 */
class HelpMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    HelpMenu(QWidget *parent = nullptr);

    /**
     * Invoked when the menu is shown
     * @param event Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

private slots:

    /** Display ManiVault About Messagebox
     * Invoked when trigger _aboutAction is clicked in the help menu
     */
    void about() const;

    /** Display Third Party About Messagebox
     * Invoked when trigger _aboutThirdParties is clicked in the help menu
     */
    void aboutThirdParties() const;
    
private:
    
    /**
     * Populate the menu
     */
    void populate ();

private:
    mv::gui::TriggerAction  _devDocAction;                /** Menu entry for ManiVault About Messagebox */
    mv::gui::TriggerAction  _aboutProjectAction;          /** Action for triggering the ManiVault about project splash screen */
    mv::gui::TriggerAction  _aboutAction;                 /** Menu entry for ManiVault About Messagebox */
    mv::gui::TriggerAction  _aboutQtAction;               /** Menu entry for Qt About Messagebox */
    mv::gui::TriggerAction  _aboutThirdPartiesAction;     /** Menu entry for Third Party About Messagebox */
};
