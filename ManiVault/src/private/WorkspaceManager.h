// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DockManager.h"
#include "ViewPluginsDockWidget.h"

#include <AbstractWorkspaceManager.h>

#include <actions/RecentFilesAction.h>

class QMainWindow;

namespace mv
{

/**
 * Workspace manager class
 *
 * Manages the placement of view plugins on the screen using the Advanced Dock System (ADS) for Qt
 * 
 * It distinguishes between built-in (system) view plugins (data hierarchy, data properties etc.) and other view plugins (scatter plot, image viewer etc.).
 * 
 * It has a central area where non-system view plugins are docked, and areas around it (left, right, top and bottom) where
 * system view plugins are docked.
 * 
 * It uses two ADS dock managers:
 * - One for the main layout (WorkspaceManager#_dockManager) 
 * - One for the docking of non-system view plugins in the central area dock widget (WorkspaceManager#_viewPluginsDockManager)
 * 
 * @author Thomas Kroes
 */
class WorkspaceManager final : public AbstractWorkspaceManager
{
    Q_OBJECT

public:

    /** View menu options */
    enum ViewMenuOption {
        LoadSystemViewPlugins   = 0x00001,      /** Show menu for loading system views */
        LoadViewPlugins         = 0x00002,      /** Show menu for loading views */
        LoadedViewsSubMenu      = 0x00004,      /** Show menu for toggling loaded views visibility */

        Default = LoadSystemViewPlugins | LoadViewPlugins | LoadedViewsSubMenu
    };

    Q_DECLARE_FLAGS(ViewMenuOptions, ViewMenuOption)

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    WorkspaceManager(QObject* parent);

    /** Reset when destructed */
    ~WorkspaceManager() override;

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the workspace manager */
    void reset() override;

    /**
     * Get manager icon
     * @return Icon
     */
    QIcon getIcon() const override;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param dockToViewPlugin Pointer to view plugin to which the view plugin will be docked (docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right) override;

    /**
     * Add a \p viewPlugin on top of the main window
     * @param viewPlugin Pointer to view plugin to add
     */
    void addViewPluginFloated(plugin::ViewPlugin* viewPlugin) override;

    /**
     * Set whether \p viewPlugin is isolated or not (closes all other view plugins when isolated)
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param isolate Whether to isolate \p viewPlugin or to reset the view layout prior to isolation
     */
    void isolateViewPlugin(plugin::ViewPlugin* viewPlugin, bool isolate) override;

    /**
     * Get workspace widget
     * @return Pointer to workspace widget
     */
    QWidget* getWidget() override;

public: // View plugin dock widget permissions

    /**
     * Freeze all dock widgets with \p dockWidgetPermissions
     * @param dockWidgetPermissions Global dock widget permission flags
     * @param set Whether to set or unset the dock widget permission flags
     */
    void setViewPluginDockWidgetPermissionsGlobally(const util::DockWidgetPermissions& dockWidgetPermissions = util::DockWidgetPermission::All, bool set = true) override;

public: // Miscellaneous

    /**
     * Get whether the native widgets have been warmed up already, to prevent multiple warmups
     * @return Boolean indicating whether the native widgets have been warmed up already
     */
    bool hasWarmedUpNativeWidgets() const override;

public: // Locking

    /**
     * Get whether the manager be locked
     * @return Boolean determining whether the manager be locked
     */
    bool mayLock() const override;

    /**
     * Get whether the manager be unlocked
     * @return Boolean determining whether the manager be unlocked
     */
    bool mayUnlock() const override;

public: // IO

    /** Creates a new workspace */
    void newWorkspace() override;

    /**
     * Load a workspace from disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    void loadWorkspace(QString filePath = "", bool addToRecentWorkspaces = true) override;

    /**
     * Import a workspace from a project file
     * @param projectFilePath File path of the project (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    void importWorkspaceFromProjectFile(QString projectFilePath = "", bool addToRecentWorkspaces = true) override;

    /**
     * Save a workspace to disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    void saveWorkspace(QString filePath = "", bool addToRecentWorkspaces = true) override;

    /** Save a workspace to disk on a different location */
    void saveWorkspaceAs() override;

    /**
     * Get whether a workspace exists
     * @return Boolean determining whether a workspace exists
     */
    bool hasWorkspace() const override;

    /**
     * Get current workspace
     * @return Pointer to current workspace (nullptr if no workspace is loaded)
     */
    const Workspace* getCurrentWorkspace() const override;

    /**
     * Get current workspace
     * @return Pointer to current workspace (nullptr if no workspace is loaded)
     */
    Workspace* getCurrentWorkspace() override;

    /**
     * Get workspace locations for location \p types
     * @return List of workspace locations
     */
    WorkspaceLocations getWorkspaceLocations(const WorkspaceLocation::Types& types = WorkspaceLocation::Type::All) override;

    /**
     * Get view plugin names from \p workspaceJsonFile
     * @param workspaceJsonFile Location of the workspace JSON file
     */
    QStringList getViewPluginNames(const QString& workspaceJsonFile) const override;

    /**
     * Get a preview image
     * @return Workspace manager preview image
     */
    QImage toPreviewImage() const override;

public: // Serialization

    /**
     * Load layout from variant
     * @param variantMap Variant representation of the manager
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save layout to variant
     * @return Variant representation of the layout
     */
    QVariantMap toVariantMap() const override;

public: // Menus

    /**
     * Get workspace menu
     * @param parent Pointer to parent menu
     * @return Pointer to created menu
     */
    QMenu* getMenu(QWidget* parent = nullptr) override;

private:

    /** Creates a new workspace */
    void createWorkspace();

    /** Create custom icon for the manager */
    void createIcon();

public: // Action getters

    gui::TriggerAction& getEditWorkspaceSettingsAction() { return _editWorkspaceSettingsAction; }

private:
    QScopedPointer<Workspace>           _workspace;                             /** Current workspace */
    QPointer<DockManager>               _mainDockManager;                       /** Dock manager for docking system view plugins */
    QPointer<DockManager>               _viewPluginsDockManager;                /** Dock manager for docking non-system view plugins */
    QPointer<ViewPluginsDockWidget>     _viewPluginsDockWidget;                 /** Pointer to view plugins dock widget in which non-system view plugins are docked */
    gui::TriggerAction                  _resetWorkspaceAction;                  /** Action for resetting the current workspace */
    gui::TriggerAction                  _importWorkspaceAction;                 /** Action for importing a workspace from file */
    gui::TriggerAction                  _exportWorkspaceAction;                 /** Action for exporting the current workspace to file */
    gui::TriggerAction                  _exportWorkspaceAsAction;               /** Action for exporting the current workspace to file with a different path */
    gui::TriggerAction                  _editWorkspaceSettingsAction;           /** Action for triggering the workspace settings dialog */
    gui::TriggerAction                  _importWorkspaceFromProjectAction;      /** Action for importing a workspace from a project file */
    gui::RecentFilesAction              _recentWorkspacesAction;                /** Action for saving the current workspace to file with a different path */
    QIcon                               _icon;                                  /** Manager icon */
    QString                             _styleSheet;                            /** Dock manager style sheet */
};

}
