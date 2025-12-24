// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractProjectManager.h>

#include <models/ProjectsListModel.h>
#include <models/ProjectsTreeModel.h>

#include <QObject>

namespace mv
{

/**
 * Project manager class
 *
 * Class for managing the creation/loading/saving of projects and contains a single project instance.
 *
 * @author Thomas Kroes
 */
class ProjectManager final : public mv::AbstractProjectManager
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    ProjectManager(QObject* parent);

    /** Reset when destructed */
    ~ProjectManager() override;

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the project manager */
    void reset() override;

    /**
     * Creates a new project, possibly also loading a workspace located at \p workspaceFilePath
     * @param workspaceFilePath File path of the workspace to load (will not attempt to load workspace if empty)
     */
    void newProject(const QString& workspaceFilePath = "") override;

    /**
     * Creates a new project with default system view plugins (either on the left or right, other alignments are ignored)
     * @param alignment Alignment of the default plugins
     * @param logging Whether to add a logging view at the bottom
     */
    void newProject(const Qt::AlignmentFlag& alignment, bool logging = false) override;

    /** Creates a new blank project (without any view plugins or data) */
    void newBlankProject() override;

    /**
     * Open project from \p filePath
     * @param filePath File path of the project (choose file path when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied by the project
     */
    void openProject(QString filePath = "", bool importDataOnly = false, bool loadWorkspace = true) override;

    /**
     * Download project from \p url, store it in \p targetDir and open it
     * @param url URL of the project
     * @param targetDirectory Directory where the project is stored (temporary directory when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied by the project
     */
    void openProject(QUrl url, const QString& targetDirectory = "", bool importDataOnly = false, bool loadWorkspace = true) override;

    /**
     * Download project from \p project model, store it in \p targetDir and open it
     * @param project Shared pointer to the project model project
     * @param targetDirectory Directory where the project is stored (temporary directory when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied by the project
     */
    void openProject(util::ProjectsModelProjectSharedPtr project, const QString& targetDirectory = "", bool importDataOnly = false, bool loadWorkspace = true) override;

    /**
     * Import project from \p filePath (only import the data)
     * @param filePath File path of the project (choose file path when empty)
     */
    void importProject(QString filePath = "") override;

    /**
     * Save a project to \p filePath
     * @param filePath File path of the project (choose file path when empty)
     * @param password Encryption password
     */
    void saveProject(QString filePath = "", const QString& password = "") override;

    /** Save project to different file (user is prompted to choose the file location) */
    void saveProjectAs() override;

    /**
     * Publish project to \p filePath
     * @param filePath File path of the published project
     */
    void publishProject(QString filePath = "") override;

    /**
     * Get whether a project exists
     * @return Boolean determining whether a project exists
     */
    bool hasProject() const override;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    const mv::Project* getCurrentProject() const override;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    mv::Project* getCurrentProject() override;

    /**
     * Extract \p filePath from compressed ManiVault project in \p maniVaultFilePath
     * @param maniVaultFilePath File path of the compressed ManiVault file
     * @param temporaryDir Temporary directory where the extracted file resides
     * @param filePath Relative file path of the file that needs to be extracted
     * @return File path of the extracted file, empty string if extraction failed
     */
    QString extractFileFromManiVaultProject(const QString& maniVaultFilePath, const QTemporaryDir& temporaryDir, const QString& filePath) override;

    /**
     * Get preview image of the project workspace
     * @param projectFilePath Path of the project file
     * @param targetSize Target size of the preview image
     * @return Preview image
     */
    QImage getWorkspacePreview(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const override;

    /**
     * Get projects list model
     * @return Reference to the projects list model
     */
    const ProjectsListModel& getProjectsListModel() const override;

    /**
     * Get projects tree model
     * @return Reference to the projects tree model
     */
    const ProjectsTreeModel& getProjectsTreeModel() const override;

    /**
     * Download project asynchronously from \p url and store it in the default downloaded projects directory
     * @param url URL of the project to download
     * @param targetDirectory Directory where the project is stored (default is empty, which means the default downloaded projects directory)
     * @param task Optional task to associate with the download operation (must live in the main/GUI thread)
     * @return Future containing the path to the downloaded project file
     */
    QFuture<QString> downloadProjectAsync(QUrl url, const QString& targetDirectory = "", Task* task = nullptr) override;

    /**
     * Establish asynchronously whether the project at \p url is updated with respect to the last downloaded version
     * @return Future containing a boolean determining whether the project is stale (true) or not (false)
     */
    QFuture<bool> isDownloadedProjectStaleAsync(QUrl url) const override;

    /**
     * Get the directory where downloaded projects are stored
     * @return Directory where downloaded projects are stored
     */
    QDir getDownloadedProjectsDir() const override;

public: // Menus

    /**
     * Get new project menu
     * @return Pointer to new project menu
     */
    QMenu& getNewProjectMenu() override;

    /**
     * Get import data menu
     * @return Pointer to import data menu
     */
    QMenu& getImportDataMenu() override;

public: // Miscellaneous

    /**
     * Get project meta action for the project with \p projectFilePath
     * @param projectFilePath File path of the project for which to get the meta action
     * @return Pointer to the project meta action, or nullptr if no action is found
     */
    ProjectMetaAction* getProjectMetaAction(const QString& projectFilePath) override;

    /**
     * Determine whether \p name is a ManiVault project file name
     * @param name File name to check
     * @return Boolean determining whether the file name is a ManiVault project file name
     */
    static bool isMvFileName(const QString& name);

private:

    /** Resets the manager and creates a new project */
    void createProject();

    /**
     * Download project from \p url, store it in \p targetDir and open it
     * @param url URL of the project
     * @param targetDirectory Directory where the project is stored (temporary directory when empty)
     */
    void downloadAndOpenProject(QUrl url, const QString& targetDirectory) const;

    /**
     * Resolve project file name from \p url asynchronously (in case of redirecting URL)
     * @param url URL of the project
     * @return Future containing the resolved project file name
     */
    static QFuture<QString> resolveProjectFileNameAsync(const QUrl& url);

public: // Serialization

    /**
     * Load widget action from variant
     * @param variantMap Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    mv::gui::TriggerAction& getNewBlankProjectAction() override { return _newBlankProjectAction; }
    mv::gui::TriggerAction& getNewProjectFromWorkspaceAction() override { return _newProjectFromWorkspaceAction; }
    mv::gui::TriggerAction& getOpenProjectAction() override { return _openProjectAction; }
    mv::gui::TriggerAction& getImportProjectAction() override { return _importProjectAction; };
    mv::gui::TriggerAction& getSaveProjectAction() override { return _saveProjectAction; }
    mv::gui::TriggerAction& getSaveProjectAsAction() override { return _saveProjectAsAction; }
    mv::gui::TriggerAction& getEditProjectSettingsAction() override { return _editProjectSettingsAction; }
    mv::gui::RecentFilesAction& getRecentProjectsAction() override { return _recentProjectsAction; }
    mv::gui::TriggerAction& getPublishAction() override { return _publishAction; }
    mv::gui::TriggerAction& getPluginManagerAction() override { return _pluginManagerAction; }
    mv::gui::ToggleAction& getShowStartPageAction() override { return _showStartPageAction; }
    mv::gui::TriggerAction& getBackToProjectAction() override { return _backToProjectAction; }

private:
    QScopedPointer<mv::Project>     _project;                           /** Current project */
    gui::TriggerAction              _newBlankProjectAction;             /** Action for creating a blank project (without view plugins and data) */
    gui::TriggerAction              _newProjectFromWorkspaceAction;     /** Action for creating a new project from a workspace */
    gui::TriggerAction              _openProjectAction;                 /** Action for opening a project */
    gui::TriggerAction              _importProjectAction;               /** Action for importing a project */
    gui::TriggerAction              _saveProjectAction;                 /** Action for saving a project */
    gui::TriggerAction              _saveProjectAsAction;               /** Action for saving a project under a new name */
    gui::TriggerAction              _editProjectSettingsAction;         /** Action for triggering the project settings dialog */
    gui::RecentFilesAction          _recentProjectsAction;              /** Menu for loading recent projects */
    QMenu                           _newProjectMenu;                    /** Menu for creating a new project */
    QMenu                           _importDataMenu;                    /** Menu for importing data */
    gui::TriggerAction              _publishAction;                     /** Action for publishing the project to an end-user */
    gui::TriggerAction              _pluginManagerAction;               /** Action for showing the loaded plugins dialog */
    gui::ToggleAction               _showStartPageAction;               /** Action for toggling the start page */
    gui::TriggerAction              _backToProjectAction;               /** Action for going back to the project */
    ProjectsListModel               _projectsListModel;                 /** Projects list model */
    ProjectsTreeModel               _projectsTreeModel;                 /** Projects tree model */
};

}