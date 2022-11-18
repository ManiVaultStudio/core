#pragma once

#include "DockWidget.h"
#include "LogoWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>

/**
 * Visualization widget class
 *
 * Widget class which contains a docking manager for docking view plugins (non-standard)
 *
 * @author Thomas Kroes
 */
class VisualizationDockWidget : public DockWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    VisualizationDockWidget(QWidget* parent = nullptr);

    /**
     * Get dock manager
     * @return Shared pointer to dock manager
     */
    DockManager& getDockManager();

    /**
     * Get dock manager
     * @return Shared pointer to dock manager
     */
    const DockManager& getDockManager() const;

    /**
     * Add view plugin
     * @param viewPluginDockWidget Pointer to view plugin dock widget
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which new view plugins will be docked
     */
    void addViewPlugin(ViewPluginDockWidget* viewPluginDockWidget, hdps::plugin::ViewPlugin* dockToViewPlugin, hdps::gui::DockAreaFlag dockArea);

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

    /**
     * Get number of open view plugin dock widgets
     * @return Number of open view plugin dock widgets
     */
    std::int32_t getNumberOfOpenViewPluginDockWidgets() const;

    /** Resets the visualization dock manager, it removes all view plugin dock widgets */
    void reset();

    /**
     * Find the dock area widget where the widget of \p viewPlugin resides
     * @param viewPlugin Pointer to view plugin that holds the widget
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(hdps::plugin::ViewPlugin* viewPlugin);

private:
    QWidget                         _widget;                    /** Widget for display in the dock widget */
    DockManager                     _dockManager;               /** Dock manager */
    ads::CDockAreaWidget*           _centralDockArea;           /** Pointer to central dock area widget */
    DockWidget                      _centralDockWidget;         /** Logo dock widget (dock manager central widget, visible when no other dock widgets are visible) */
    LogoWidget                      _logoWidget;                /** Logo widget for logo dock widget */
    ads::CDockAreaWidget*           _lastDockAreaWidget;        /** Last docking area widget (if any) */
    ViewPluginDockWidgets           _viewPluginDockWidgets;     /** Added view plugin dock widgets */
};
