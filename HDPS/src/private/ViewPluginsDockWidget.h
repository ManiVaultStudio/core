#pragma once

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
class ViewPluginsDockWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ViewPluginsDockWidget(QWidget* parent = nullptr);

    /**
     * Get dock manager
     * @return Reference to the dock manager
     */
    DockManager& getDockManager();

    /**
     * Get dock manager
     * @return Reference to the dock manager
     */
    const DockManager& getDockManager() const;

    /**
     * Add view plugin
     * @param viewPluginDockWidget Pointer to view plugin dock widget
     * @param dockViewPlugin Pointer to view plugin to which the dock widget will be docked (docked top-level if nullptr)
     * @param dockArea Dock area in which the dock widget will be docked
     */
    void addViewPlugin(ViewPluginDockWidget* viewPluginDockWidget, hdps::plugin::ViewPlugin* dockToViewPlugin, hdps::gui::DockAreaFlag dockArea);

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

    /**
     * Get number of open view plugin dock widgets
     * @return Number of open view plugin dock widgets
     */
    std::int32_t getNumberOfOpenViewPluginDockWidgets() const;

    /**
     * Find the dock area widget where the widget of \p viewPlugin resides
     * @param viewPlugin Pointer to view plugin that holds the widget
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(hdps::plugin::ViewPlugin* viewPlugin);

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

private:
    DockManager         _dockManager;           /** Dock manager for docking of view plugins */
    ads::CDockWidget    _centralDockWidget;     /** Central dock widget (show when no view plugins are visible) */
    LogoWidget          _logoWidget;            /** Logo widget for logo dock widget */
};
