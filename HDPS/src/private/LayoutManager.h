#pragma once

#include <AbstractLayoutManager.h>

#include "DockManager.h"
#include "VisualizationDockWidget.h"

#include <DockAreaWidget.h>

#include <QSharedPointer>

class QMainWindow;

namespace hdps::gui
{

class LayoutManager : public AbstractLayoutManager
{
    Q_OBJECT

public:

    /** Default constructor */
    LayoutManager();

    /** Default destructor */
    ~LayoutManager() override;

    /**
     * Initializes the layout manager to work with the main window
     * @param mainWindow Pointer to main window to apply the layout manager to
     */
    void initialize(QMainWindow* mainWindow) override;

    /**
     * Resets the layout to defaults
     */
    void reset() override;

    /**
     * Add a view plugin to the top-level layout
     * @param viewPlugin Pointer to view plugin
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin) override;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which new view plugins will be docked
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockViewPlugin, plugin::ViewPlugin::DockArea dockArea) override;

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

protected: // Docking

    /**
     * Get dock view plugin
     * @return Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    plugin::ViewPlugin* getDockViewPlugin() const override;

    /**
     * Set dock view plugin
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    void setDockViewPlugin(plugin::ViewPlugin* dockViewPlugin) override;

    /**
     * Get dock area
     * @return Dock area in which new view plugins will be docked
     */
    plugin::ViewPlugin::DockArea getDockArea() const override;

    /**
     * Set dock area
     * @param dockArea Dock area in which new view plugins will be docked
     */
    void setDockArea(plugin::ViewPlugin::DockArea dockArea) override;

private:
    QSharedPointer<DockManager>     _dockManager;                   /** Dock manager (inherited  from ADS) */
    ads::CDockAreaWidget*           _visualizationDockArea;         /** Visualization docking area for view plugins (non-standard) */
    VisualizationDockWidget         _visualizationDockWidget;       /** Visualization dock widget for view plugins (non-standard) */
    plugin::ViewPlugin*             _dockViewPlugin;                /** Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr) */
    plugin::ViewPlugin::DockArea    _dockArea;                      /** Dock area in which new view plugins will be docked */
};

}
