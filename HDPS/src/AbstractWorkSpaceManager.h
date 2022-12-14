#pragma once

#include "Workspace.h"

#include <actions/TriggerAction.h>

#include <QObject>
#include <QScopedPointer>

/**
 * Workspace manager class
 *
 * TODO: Add description
 *
 * @author Thomas Kroes
 */
class WorkspaceManager final : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct workspace manager with \p parent object
     * @param parent Pointer to parent object
     */
    WorkspaceManager(QObject* parent = nullptr);

    /**
     * Load a workspace from disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    void loadWorkspace(QString filePath = "");

    /**
     * Save a workspace to disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    void saveWorkspace(QString filePath);

    /** Save a workspace to disk on a different location */
    void saveWorkspaceAs();

    /**
     * Get workspace menu
     * @param menu Pointer to parent menu
     * @return Pointer to created menu
     */
    QMenu* getMenu(QWidget* parent = nullptr);

    /** Create custom icon for the manager */
    void createIcon();

private:

    /** Creates a new workspace and assigns it to the current workspace */
    void createWorkspace();

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

private:
    QScopedPointer<Workspace>   _workspace;                 /** Current workspace */
    hdps::gui::TriggerAction    _loadWorkspaceAction;       /** Action for loading a workspace from file */
    hdps::gui::TriggerAction    _saveWorkspaceAction;       /** Action for saving the current workspace to file */
    hdps::gui::TriggerAction    _saveWorkspaceAsAction;     /** Action for saving the current workspace to file with a different path */
    QIcon                       _icon;                      /** Manager icon */
};
