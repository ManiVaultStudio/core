#pragma once

#include "LogoWidget.h"

#include <DockWidget.h>
#include <DockManager.h>

#include <QWidget>

/**
 * Visualization widget class
 *
 * Widget class which contains a docking manager for docking view plugins (non-standard)
 *
 * @author Thomas Kroes
 */
class VisualizationDockWidget : public ads::CDockWidget
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
    ads::CDockManager& getDockManager();

    /**
     * Add view plugin
     * @param viewPluginDockWidget Pointer to view plugin dock widget
     */
    void addViewPlugin(ads::CDockWidget* viewPluginDockWidget);

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

private:
    QWidget                         _widget;
    ads::CDockManager               _dockManager;               /** Dock manager */
    ads::CDockAreaWidget*           _centralDockArea;           /** Pointer to central dock area widget */
    ads::CDockWidget                _centralDockWidget;         /** Logo dock widget (dock manager central widget, visible when no other dock widgets are visible) */
    LogoWidget                      _logoWidget;                /** Logo widget for logo dock widget */
    ads::CDockAreaWidget*           _lastDockAreaWidget;        /** Last docking area widget (if any) */
    QVector<ads::CDockWidget*>      _viewPluginDockWidgets;     /** Added view plugin dock widgets */
};
