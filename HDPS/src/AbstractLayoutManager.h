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
     * Add a view plugin to the layout
     * @param viewPlugin Pointer to view plugin
     */
    virtual void addViewPlugin(plugin::ViewPlugin* viewPlugin) = 0;

};

}
