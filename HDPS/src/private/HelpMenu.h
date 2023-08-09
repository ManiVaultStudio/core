// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QMenu>

namespace hdps::gui {
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

private slots:

    /** Display ManiVault About Messagebox
     * Invoked when trigger _aboutAction is clicked in the help menu
     */
    void about();

    /** Display Third Party About Messagebox
     * Invoked when trigger _aboutThirdParties is clicked in the help menu
     */
    void aboutThirdParties();
    
private:
    
    /**
     * Populate the menu
     */
    void populate ();

private:
    
    hdps::gui::TriggerAction* _devDocAction;        /** Menu entry for ManiVault About Messagebox */
    hdps::gui::TriggerAction* _aboutAction;         /** Menu entry for ManiVault About Messagebox */
    hdps::gui::TriggerAction* _aboutQt;             /** Menu entry for Qt About Messagebox */
    hdps::gui::TriggerAction* _aboutThirdParties;   /** Menu entry for Third Party About Messagebox */
};
