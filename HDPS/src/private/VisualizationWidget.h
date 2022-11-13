#pragma once

#include "LogoWidget.h"

#include <DockManager.h>

#include <QWidget>

/**
 * Visualization widget class
 *
 * Widget class which contains a docking manager for docking view plugins (non-standard)
 *
 * @author Thomas Kroes
 */
class VisualizationWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    VisualizationWidget(QWidget* parent = nullptr);

    /**
     * Get dock manager
     * @return Reference to dock manager
     */
    ads::CDockManager& getDockManager();

private:

    /** Updates the central widget in response to changes in docking and dock widgets visibility */
    void updateCentralWidget();

private:
    ads::CDockManager       _dockManager;           /** Dock manager */
    ads::CDockAreaWidget*   _centralDockArea;       /** Pointer to central dock area widget */
    ads::CDockWidget        _centralDockWidget;     /** Logo dock widget (dock manager central widget, visible when no other dock widgets are visible) */
    LogoWidget              _logoWidget;            /** Logo widget for logo dock widget */
};
