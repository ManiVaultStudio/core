#pragma once

#include <DockWidget.h>

#include "CentralDockWidget.h"
#include "LogoWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

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
    ViewPluginsDockWidget(QPointer<DockManager> dockManager, QWidget* parent = nullptr);

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

    /**
     * Invoked when a dock widget is added
     * @param dockWidget Pointer to added dock widget
     */
    void dockWidgetAdded(ads::CDockWidget* dockWidget);

    /**
     * Invoked when a dock widget is about to be removed
     * @param dockWidget Pointer to about to be removed dock widget
     */
    void dockWidgetAboutToBeRemoved(ads::CDockWidget* dockWidget);

public: // Cache and restore visibility for view plugin isolation

    /**
     * Set \p viewPlugin isolation to \p isolate
     * @param viewPlugin View plugin to isolate
     * @param isolate Whether to isolate the view plugin or not
     */
    static void isolate(hdps::plugin::ViewPlugin* viewPlugin, bool isolate);

    /** Caches the visibility of each view plugin */
    static void cacheVisibility();

    /** Restores the visibility of each view plugin */
    static void restoreVisibility();

private:
    QPointer<DockManager>           _dockManager;                   /** Dock manager for docking of view plugins */
    ads::CDockWidget                _centralDockWidget;             /** Central dock widget (show when no view plugins are visible) */
    LogoWidget                      _logoWidget;                    /** Logo widget for logo dock widget */
};
