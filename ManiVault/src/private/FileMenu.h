// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/TriggerAction.h>

#include <QMenu>

/**
 * File menu class
 * 
 * Menu class for file-related operations
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
     * 
     * @param event 
     */
    void showEvent(QShowEvent* event) override;

private:
    
    /**
     * Populate the menu
     */
    void populate ();

private:
    mv::gui::TriggerAction    _exitApplictionAction;  /** Trigger action to exit the application */
};
