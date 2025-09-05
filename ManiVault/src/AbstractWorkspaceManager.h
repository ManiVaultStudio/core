// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Workspace.h"

#include "ViewPlugin.h"

#include "util/DockWidgetPermission.h"

#ifdef _DEBUG
    //#define ABSTRACT_WORKSPACE_MANAGER_VERBOSE
#endif

namespace mv
{

/** Workspace location which consists of a workspace title, file path and location type */
class CORE_EXPORT WorkspaceLocation final {
public:

    /** Enum determining the type of workspace location */
    enum Type {
        BuiltIn = 0x0001,   /** Built-in workspaces (located in /workspaces in the application dir) */
        Path    = 0x0002,   /** Workspaces on the global workspaces path */
        Recent  = 0x0004,   /** Recent workspaces */

        All = BuiltIn | Path | Recent
    };

    Q_DECLARE_FLAGS(Types, Type)

    /** Map enum type to name */
    static inline QMap<Type, QString> typeNames{
        { BuiltIn, "Built-in" },
        { Path, "Path" },
        { Recent, "Recent" },
        { All, "All" }
    };

    /**
     * Get name of \p type
     * @return Type name
     */
    static QString getTypeName(const Type& type) {
        return typeNames[type];
    }

public:

    /**
     * Construct workspace location with \p filePath and \p type
     * @param title Title of the workspace
     * @param filePath Location of the workspace on disk
     * @param type Location type of the workspace
     */
    explicit WorkspaceLocation(const QString& title, const QString& filePath, const Type& type) :
        _title(title),
        _filePath(filePath),
        _type(type)
    {
    }

    /**
     * Get file path of the workspace
     * @return File path of the workspace
     */
    QString getTitle() const {
        return _title;
    }

    /**
     * Get file path of the workspace
     * @return File path of the workspace
     */
    QString getFilePath() const {
        return _filePath;
    }

    /**
     * Get type of workspace location
     * @return Type
     */
    Type getType() const {
        return _type;
    }

    /**
     * Get type name of workspace location
     * @return Type name
     */
    QString getTypeName() const {
        return getTypeName(_type);
    }

private:
    QString   _title;       /** Title of the workspace */
    QString   _filePath;    /** File path of the workspace */
    Type      _type;        /** Type of workspace location */
};

using WorkspaceLocations = QList<WorkspaceLocation>;

/**
 * Abstract workspace manager class
 *
 * Base abstract workspace manager class for managing workspaces and layout.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractWorkspaceManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractWorkspaceManager(QObject* parent) :
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
     * Add a \p viewPlugin on top of the main window
     * @param viewPlugin Pointer to view plugin to add
     */
    virtual void addViewPluginFloated(plugin::ViewPlugin* viewPlugin) = 0;

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
     * @param parent Pointer to parent menu
     * @return Pointer to created menu
     */
    virtual QMenu* getMenu(QWidget* parent = nullptr) = 0;

    /**
     * Get the file path of the loaded workspace
     * @return File path of the loaded workspace
     */
    QString getWorkspaceFilePath() const {
        if (!hasWorkspace())
            return "";

        return getCurrentWorkspace()->getFilePath();
    }

    /**
     * Set the file path of the loaded workspace
     * @param filePath File path of the loaded workspace
     */
    void setWorkspaceFilePath(const QString& filePath) {
        if (!hasWorkspace())
            return;

        if (filePath == getCurrentWorkspace()->getFilePath())
            return;

#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        getCurrentWorkspace()->setFilePath(filePath);

        emit workspaceFilePathChanged(getCurrentWorkspace()->getFilePath());
    }

    /** Begin the workspace loading process */
    void beginLoadWorkspace() {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        Q_ASSERT(getCurrentWorkspace() != nullptr);

        if (!hasWorkspace())
            return;

        emit workspaceAboutToBeLoaded(getCurrentWorkspace()->getFilePath());
    }

    /** End the workspace loading process */
    void endLoadWorkspace() {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        Q_ASSERT(getCurrentWorkspace() != nullptr);

        if (!hasWorkspace())
            return;

        emit workspaceLoaded(getCurrentWorkspace()->getFilePath());
    }

    /** Begin the workspace saving process */
    void beginSaveWorkspace() {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        Q_ASSERT(getCurrentWorkspace() != nullptr);

        if (!hasWorkspace())
            return;

        emit workspaceAboutToBeSaved(getCurrentWorkspace()->getFilePath());
    }

    /** End the workspace saving process */
    void endSaveWorkspace() {
#ifdef ABSTRACT_WORKSPACE_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        Q_ASSERT(getCurrentWorkspace() != nullptr);

        if (!hasWorkspace())
            return;

        emit workspaceSaved(getCurrentWorkspace()->getFilePath());
    }

    /** Applies the CSS stylesheet to the dock managers 
    virtual void applyStyleSheet() final {
        QFile styleSheetFile(":/styles/ads_light.css");

        styleSheetFile.open(QIODevice::ReadOnly);

        QTextStream styleSheetStream(&styleSheetFile);
        auto stylesheet = styleSheetStream.readAll();

        styleSheetFile.close();

        _mainDockManager->setStyleSheet(stylesheet);
        auto dockManagers = findChildren<CDockManager*>();

        for (auto dockManager : dockManagers)
            dockManager->setStyleSheet(stylesheet);
    }
    */

public: // Miscellaneous

    /**
     * Get whether the native widgets have been warmed up already, to prevent multiple warmups
     * @return Boolean indicating whether the native widgets have been warmed up already
     */
    virtual bool hasWarmedUpNativeWidgets() const = 0;

public: // View plugin dock widget permissions

    /**
     * Set \p dockWidgetPermissions for all view plugin dock widgets
     * @param dockWidgetPermissions Global dock widget permission flags
     * @param set Whether to set or unset the dock widget permission flags
     */
    virtual void setViewPluginDockWidgetPermissionsGlobally(const util::DockWidgetPermissions& dockWidgetPermissions = util::DockWidgetPermission::All, bool set = true) = 0;

public: // IO

    /** Creates a new workspace */
    virtual void newWorkspace() = 0;

    /**
     * Load a workspace from disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    virtual void loadWorkspace(QString filePath = "", bool addToRecentWorkspaces = true) = 0;

    /**
     * Import a workspace from a project file
     * @param filePath File path of the project (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    virtual void importWorkspaceFromProjectFile(QString filePath = "", bool addToRecentWorkspaces = true) = 0;

    /**
     * Save a workspace to disk
     * @param filePath File path of the workspace (choose file path with dialog when empty)
     * @param addToRecentWorkspaces Whether to add the workspace file path to the recent workspace file paths
     */
    virtual void saveWorkspace(QString filePath = "", bool addToRecentWorkspaces = true) = 0;

    /** Save a workspace to disk on a different location */
    virtual void saveWorkspaceAs() = 0;

    /**
     * Get whether a workspace exists
     * @return Boolean determining whether a workspace exists
     */
    virtual bool hasWorkspace() const = 0;

    /**
     * Get current workspace
     * @return Pointer to current workspace (nullptr if no workspace is loaded)
     */
    virtual const Workspace* getCurrentWorkspace() const = 0;

    /**
     * Get current workspace
     * @return Pointer to current workspace (nullptr if no workspace is loaded)
     */
    virtual Workspace* getCurrentWorkspace() = 0;

    /**
     * Get workspace locations for location \p types
     * @return List of workspace locations
     */
    virtual WorkspaceLocations getWorkspaceLocations(const WorkspaceLocation::Types& types = WorkspaceLocation::Type::All) = 0;

    /**
     * Get view plugin names from \p workspaceJsonFile
     * @param workspaceJsonFile Location of the workspace JSON file
     */
    virtual QStringList getViewPluginNames(const QString& workspaceJsonFile) const = 0;

    /**
     * Get a preview image
     * @return Workspace manager preview image
     */
    virtual QImage toPreviewImage() const = 0;

signals:

    /** Signals that a new workspace is about to be created */
    void workspaceAboutToBeCreated();

    /**
     * Signals that \p workspace is created
     * @param workspace Reference to the created workspace
     */
    void workspaceCreated(const mv::Workspace& workspace);

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

    /**
     * Signals that the location of the current workspace changed to \p filePath
     * @param filePath File path of the workspace
     */
    void workspaceFilePathChanged(const QString& filePath);
};

}
