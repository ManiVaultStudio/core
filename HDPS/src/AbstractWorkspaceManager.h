#pragma once

#include "AbstractManager.h"

#include <ViewPlugin.h>

class QMainWindow;

#ifdef _DEBUG
    #define ABSTRACT_WORKSPACE_MANAGER_VERBOSE
#endif

namespace hdps
{

/**
 * Abstract workspace manager class
 *
 * Base abstract workspace manager class for managing workspaces and layout.
 *
 * @author Thomas Kroes
 */
class AbstractWorkspaceManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct workspace manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractWorkspaceManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Workspace")
    {
    }

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
     * Get workspace widget
     * @return Pointer to workspace widget
     */
    virtual QWidget* getWidget() = 0;

    /**
     * Get workspace menu
     * @param menu Pointer to parent menu
     * @return Pointer to created menu
     */
    virtual QMenu* getMenu(QWidget* parent = nullptr) = 0;

    /**
     * Get the file path of the loaded workspace
     * @return File path of the loaded workspace
     */
    virtual QString getWorkspaceFilePath() const final {
        return _workspaceFilePath;
    }

    /**
     * Set the file path of the loaded workspace
     * @param filePath File path of the loaded workspace
     */
    virtual void setWorkspaceFilePath(const QString& filePath) final {
        _workspaceFilePath = filePath;
    }

    /** Begin the workspace loading process */
    virtual void beginLoadWorkspace() final {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        emit workspaceAboutToBeLoaded(_workspaceFilePath);
    }

    /** End the workspace loading process */
    virtual void endLoadWorkspace() final {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        emit workspaceLoaded(_workspaceFilePath);
    }

    /** Begin the workspace saving process */
    virtual void beginSaveWorkspace() final {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        emit workspaceAboutToBeSaved(_workspaceFilePath);
    }

    /** End the workspace saving process */
    virtual void endSaveWorkspace() final {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        emit workspaceSaved(_workspaceFilePath);
    }

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
    virtual void saveWorkspace(QString filePath = "") = 0;

    /** Save a workspace to disk on a different location */
    virtual void saveWorkspaceAs() = 0;

signals:

    /**
     * Signals that a workspace is about to be loaded from \p filePath
     * @param filePath File path of the workspace
     */
    void workspaceAboutToBeLoaded(const QString& filePath);

    /**
     * Signals that a workspace is loaded from \p filePath
     * @param filePath File path of the workspace
     */
    void workspaceLoaded(const QString& filePath);

    /**
     * Signals that a workspace is about to be saved to \p filePath
     * @param filePath File path of the workspace
     */
    void workspaceAboutToBeSaved(const QString& filePath);

    /**
     * Signals that a workspace is saved to \p filePath
     * @param filePath File path of the workspace
     */
    void workspaceSaved(const QString& filePath);

private:
    QString     _workspaceFilePath;      /** File path of the current workspace */
};

}
