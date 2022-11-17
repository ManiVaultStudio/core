#pragma once

#include <actions/WidgetAction.h>

#include <ViewPlugin.h>

class QMainWindow;

namespace hdps::gui
{

class AbstractLayoutManager : public gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Initializes the layout manager to work with the main window
     * @param mainWindow Pointer to main window to apply the layout manager to
     */
    virtual void initialize(QMainWindow* mainWindow) = 0;

    /**
     * Resets the docking layout to defaults
     */
    virtual void reset() = 0;

    /**
     * Add a view plugin to the top-level layout
     * @param viewPlugin Pointer to view plugin
     */
    virtual void addViewPlugin(plugin::ViewPlugin* viewPlugin) = 0;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which new view plugins will be docked
     */
    virtual void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockViewPlugin, plugin::ViewPlugin::DockArea dockArea) = 0;

protected: // Docking

    /**
     * Get dock view plugin
     * @return Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    virtual plugin::ViewPlugin* getDockViewPlugin() const = 0;

    /**
     * Set dock view plugin
     * @param dockViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     */
    virtual void setDockViewPlugin(plugin::ViewPlugin* dockViewPlugin) = 0;

    /**
     * Get dock area
     * @return Dock area in which new view plugins will be docked
     */
    virtual plugin::ViewPlugin::DockArea getDockArea() const = 0;

    /**
     * Set dock area
     * @param dockArea Dock area in which new view plugins will be docked
     */
    virtual void setDockArea(plugin::ViewPlugin::DockArea dockArea) = 0;
};

}
