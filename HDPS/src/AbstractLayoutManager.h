#pragma once

#include "AbstractManager.h"

#include <ViewPlugin.h>

class QMainWindow;

namespace hdps::gui
{

class AbstractLayoutManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Initializes the layout manager to work with the main window
     * @param mainWindow Pointer to main window to apply the layout manager to
     */
    virtual void initialize(QMainWindow* mainWindow) = 0;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param dockToViewPlugin Pointer to view plugin to which new view plugins are docked (new view plugins be docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     */
    virtual void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right) = 0;

    /**
     * Set whether \p viewPlugin is isolated or not (closes all other view plugins when isolated)
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param isolate Whether to isolate \p viewPlugin or to reset the view layout prior to isolation
     */
    virtual void isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate) = 0;
};

}
