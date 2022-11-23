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

    /** Resets the docking layout to defaults */
    virtual void reset() = 0;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param dockToViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     */
    virtual void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right) = 0;
};

}
