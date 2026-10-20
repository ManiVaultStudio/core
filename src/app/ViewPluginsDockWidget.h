// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DockWidget.h>

#include "LogoWidget.h"
#include "DockManager.h"
#include "ViewPluginDockWidget.h"

/**
 * View plugins dock widget class
 *
 * This dock widget is placed in the center of the main docking manager.
 * All created (non-system view plugins) are docked in the docking manager of this dock widget.
 *
 * @author Thomas Kroes
 */
class ViewPluginsDockWidget : public ads::CDockWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param dockManager Pointer to docking manager owned by the workspace manager
     * @param parent Pointer to parent widget
     */
    ViewPluginsDockWidget(const QPointer<DockManager>& dockManager, QWidget* parent = nullptr);

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

    /**
     * Invoked when a dock widget is added
     * @param dockWidget Pointer to added dock widget
     */
    void dockWidgetAdded(const ads::CDockWidget* dockWidget);

    /**
     * Invoked when a dock widget is about to be removed
     * @param dockWidget Pointer to about to be removed dock widget
     */
    void dockWidgetAboutToBeRemoved(const ads::CDockWidget* dockWidget);

public: // Cache and restore visibility for view plugin isolation

    /**
     * Set \p viewPlugin isolation to \p isolate
     * @param viewPlugin View plugin to isolate
     * @param isolate Whether to isolate the view plugin or not
     */
    static void isolate(const mv::plugin::ViewPlugin* viewPlugin, bool isolate);

    /** Caches the visibility of each view plugin */
    static void cacheVisibility();

    /** Restores the visibility of each view plugin */
    static void restoreVisibility();

private:
    ads::CDockWidget                _centralDockWidget;     /** Central dock widget (show when no view plugins are visible) */
    LogoWidget                      _logoWidget;            /** Logo widget for logo dock widget */

    static QPointer<DockManager> dockManager;    /** Dock manager for docking of view plugins */
};
