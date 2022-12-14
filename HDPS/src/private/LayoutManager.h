#pragma once

#include "DockManager.h"
#include "ViewPluginsDockWidget.h"

#include <AbstractWorkspaceManager.h>

#include <DockAreaWidget.h>

#include <QSharedPointer>

class QMainWindow;

namespace hdps::gui
{

/**
 * Layout manager class
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
 * - One for the docking of non-system view plugins in the central area dock widget (\p _viewPluginsDockWidget)
 * 
 * @author Thomas Kroes
 */
class LayoutManager final : public AbstractWorkspaceManager
{
    Q_OBJECT

public:

    /** Default constructor */
    LayoutManager();

    /** Default destructor */
    ~LayoutManager() override;

    /** Perform manager startup initialization */
    void initalize() override;

    /** Resets the contents of the layout manager */
    void reset() override;

    /**
     * Initializes the layout manager to work with the \p widget
     * @param widget Pointer to the widget to apply the layout manager to
     */
    void initialize(QWidget* widget) override;

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

public: // IO

    /**
     * Load a workspace from disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    void loadWorkspace(QString filePath = "") override;

    /**
     * Save a workspace to disk
     * @param filePath File path of the existing workspace (choose file path with dialog when empty)
     */
    void saveWorkspace(QString filePath) override;

    /** Save a workspace to disk on a different location */
    void saveWorkspaceAs() override;

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

public: // Miscellaneous

    /** Get all view plugin dock widgets, both from the main and the view plugins dock widget dock managers */
    ViewPluginDockWidgets getViewPluginDockWidgets();

    /**
     * Get workspace menu
     * @param menu Pointer to parent menu
     * @return Pointer to created menu
     */
    QMenu* getMenu(QWidget* parent = nullptr);

private:

    /** Create custom icon for the manager */
    void createIcon();

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
    QPointer<DockManager>               _dockManager;                   /** Main dock manager for docking system plugins */
    QPointer<ViewPluginsDockWidget>     _viewPluginsWidget;             /** Pointer to view plugins widget in which non-system view plugins are docked */
    bool                                _initialized;                   /** Whether the layout manager is initialized or not */
    QMap<DockWidget*, bool>             _cachedDockWidgetsVisibility;   /** Cached dock widgets visibility for view plugin isolation */
    QScopedPointer<Workspace>           _workspace;                     /** Current workspace */
    hdps::gui::TriggerAction            _loadWorkspaceAction;           /** Action for loading a workspace from file */
    hdps::gui::TriggerAction            _saveWorkspaceAction;           /** Action for saving the current workspace to file */
    hdps::gui::TriggerAction            _saveWorkspaceAsAction;         /** Action for saving the current workspace to file with a different path */
    QIcon                               _icon;                          /** Manager icon */
};

}
