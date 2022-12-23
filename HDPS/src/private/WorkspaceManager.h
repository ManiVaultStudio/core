#pragma once

#include "DockManager.h"
#include "ViewPluginsDockWidget.h"

#include <AbstractWorkspaceManager.h>

#include <actions/RecentFilesAction.h>

#include <DockAreaWidget.h>

#include <QSharedPointer>

class QMainWindow;

namespace hdps
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
 * - One for the main layout (\p _dockManager) 
 * - One for the docking of non-system view plugins in the central area dock widget (\p _viewPluginsDockManager)
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

    /** Default constructor */
    WorkspaceManager();

    /** Perform manager startup initialization */
    void initalize() override;

    /** Resets the contents of the workspace manager */
    void reset() override;

    /**
     * Add a view plugin to the \p dockArea of \p dockViewPlugin
     * @param viewPlugin Pointer to view plugin to add to layout
     * @param dockToViewPlugin Pointer to view plugin to which the view plugin will be docked (docked top-level if nullptr)
     * @param dockArea Dock area in which \p dockToViewPlugin will be docked
     */
    void addViewPlugin(plugin::ViewPlugin* viewPlugin, plugin::ViewPlugin* dockToViewPlugin = nullptr, gui::DockAreaFlag dockArea = gui::DockAreaFlag::Right) override;

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

public: // IO

    /**
     * Load a workspace from disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    void loadWorkspace(QString filePath = "", bool addToRecentWorkspaces = true) override;

    /**
     * Save a workspace to disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     */
    void saveWorkspace(QString filePath = "") override;

    /** Save a workspace to disk on a different location */
    void saveWorkspaceAs() override;

    /**
     * Import a workspace from a project file
     * @param filePath File path of the project (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    void importWorkspaceFromProjectFile(QString filePath = "", bool addToRecentWorkspaces = true) override;

public: // Serialization

    /**
     * Load layout from variant
     * @param Variant representation of the layout
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
     * @param menu Pointer to parent menu
     * @return Pointer to created menu
     */
    QMenu* getMenu(QWidget* parent = nullptr);

private:

    /** Create custom icon for the manager */
    void createIcon();

    /**
     * Get a preview image
     * @return Workspace manager preview image
     */
    QImage toPreviewImage() const;

    /**
     * Get preview image enclosed in \p workspaceFilePath
     * @param workspaceFilePath Path of the workspace file
     * @return Preview image
     */
    QImage getPreviewImageFromWorkspaceFile(const QString& workspaceFilePath) const;

private:
    QPointer<DockManager>               _mainDockManager;                       /** Dock manager for docking system view plugins */
    QPointer<DockManager>               _viewPluginsDockManager;                /** Dock manager for docking non-system view plugins */
    QPointer<ViewPluginsDockWidget>     _viewPluginsDockWidget;                 /** Pointer to view plugins dock widget in which non-system view plugins are docked */
    hdps::gui::TriggerAction            _newWorkspaceAction;                    /** Action for creating a workspace */
    hdps::gui::TriggerAction            _loadWorkspaceAction;                   /** Action for loading a workspace from file */
    hdps::gui::TriggerAction            _saveWorkspaceAction;                   /** Action for saving the current workspace to file */
    hdps::gui::TriggerAction            _saveWorkspaceAsAction;                 /** Action for saving the current workspace to file with a different path */
    hdps::gui::TriggerAction            _importWorkspaceFromProjectAction;      /** Action for import a workspace from a project  file */
    hdps::gui::RecentFilesAction        _recentWorkspacesAction;                /** Action for saving the current workspace to file with a different path */
    QIcon                               _icon;                                  /** Manager icon */
};

}
