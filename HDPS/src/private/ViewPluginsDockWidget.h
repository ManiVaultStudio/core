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
 * All created (non-system view plugins) are dock in the docking manager of this dock widget.
 *
 * @author Thomas Kroes
 */
class ViewPluginsDockWidget : public CentralDockWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ViewPluginsDockWidget(QWidget* parent = nullptr);

    /**
     * Get string that describes the dock widget type
     * @return Type string
     */
    QString getTypeString() const override;

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

    /**
     * Find the dock area widget where the widget of \p viewPlugin resides
     * @param viewPlugin Pointer to view plugin that holds the widget
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(hdps::plugin::ViewPlugin* viewPlugin);

private:
    QWidget         _widget;            /** Widget for display in the dock widget */
    DockManager     _dockManager;       /** Dock manager */
    LogoWidget      _logoWidget;        /** Logo widget for logo dock widget */
};
