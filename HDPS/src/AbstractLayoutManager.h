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
     * Initializes the layout manager to work with the \p widget
     * @param widget Pointer to the widget to apply the layout manager to
     */
    virtual void initialize(QWidget* widget) = 0;

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

    /**
     * Get workspace menu
     * @param menu Pointer to parent menu
     * @return Pointer to created menu
     */
    virtual QMenu* getMenu(QWidget* parent = nullptr) = 0;

public: // IO

    /**
     * Load a workspace from disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    virtual void loadWorkspace(QString filePath = "") = 0;

    /**
     * Save a workspace to disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    virtual void saveWorkspace(QString filePath) = 0;

    /** Save a workspace to disk on a different location */
    virtual void saveWorkspaceAs() = 0;

signals:

    /**
     * Signals that \p workspace is about to be loaded
     * @param workspace Reference to the workspace that is about to be loaded
     */
    void workspaceAboutToBeLoaded(const Workspace& workspace);

    /**
     * Signals that \p workspace is loaded
     * @param workspace Reference to the workspace that is loaded
     */
    void workspaceLoaded(const Workspace& workspace);

    /**
     * Signals that \p workspace is saved
     * @param workspace Reference to the saved workspace
     */
    void workspaceSaved(const Workspace& workspace);

    /**
     * Signals that a workspace is created
     * @param workspace Reference to the newly created workspace
     */
    void workspaceCreated(const Workspace& workspace);

    /**
     * Signals that a workspace is about to be destroyed
     * @param workspace Reference to the workspace that is about to be destroyed
     */
    void workspaceAboutToBeDestroyed(const Workspace& workspace);

    /**
     * Signals that a workspace is destroyed
     * @param workspaceId Globally unique identifier of the workspace that is destroyed
     */
    void workspaceDestroyed(const QString& workspaceId);
};

}
