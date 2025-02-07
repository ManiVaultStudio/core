// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DockAreaTitleBar.h>

#include <QToolButton>
#include <QMenu>

class ViewMenu;
class LoadSystemViewMenu;

/**
 * Dock area title bar
 * 
 * Extends the ADS dock area title bar in order to add a settings hamburger menu tot the title bar.
 * 
 * @author Thomas Kroes
 */
class DockAreaTitleBar : public ads::CDockAreaTitleBar
{
public:

    /**
     * Construct with \p dockAreaWidget
     * @param dockAreaWidget Pointer to owning dock area widget
     */
    DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget);
    
protected:
    /**
     * Override QObject's event handling
     * @return Boolean Whether the event was recognized and processed
     */
    bool event(QEvent* event) override;

public: // Themes
    
    /** refresh the widget and its children according to new style */
    void updateStyle();

private:
    QToolButton*            _addViewPluginToolButton;   /** Tool button which is added to the dock area title bar to add new (system) views */
    ViewMenu*               _viewMenu;                  /** Menu for loading view plugins, which is added as soon as the first dock widget is added (will only be initialized for the view plugins dock manager) */
    LoadSystemViewMenu*     _loadSystemViewMenu;        /** Menu for loading system view plugins, which is added as soon as the first dock widget is added (will only be initialized for the system plugins dock manager) */
};
