#pragma once

#include "DockWidget.h"
#include "LogoWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "ViewPluginDockWidget.h"

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
     * @return Reference to dock manager
     */
    DockManager& getDockManager();

    /**
     * Add view plugin
     * @param viewPluginDockWidget Pointer to view plugin dock widget
     */
    void addViewPlugin(ViewPluginDockWidget* viewPluginDockWidget);

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

private:
    QWidget                     _widget;                    /** Widget for display in the dock widget */
    DockManager                 _dockManager;               /** Dock manager */
    ads::CDockAreaWidget*       _centralDockArea;           /** Pointer to central dock area widget */
    DockWidget                  _centralDockWidget;         /** Logo dock widget (dock manager central widget, visible when no other dock widgets are visible) */
    LogoWidget                  _logoWidget;                /** Logo widget for logo dock widget */
    ads::CDockAreaWidget*       _lastDockAreaWidget;        /** Last docking area widget (if any) */
    ViewPluginDockWidgets       _viewPluginDockWidgets;     /** Added view plugin dock widgets */
};
