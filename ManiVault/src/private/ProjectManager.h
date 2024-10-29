// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <AbstractProjectManager.h>

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
     * Creates a new project manager from with \p parent object
     * @param parent Pointer to parent object
     */
    ProjectManager(QObject* parent = nullptr);

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
     * @param loadWorkspace Whether to load the workspace which is accompanied with the project
     */
    void openProject(QString filePath = "", bool importDataOnly = false, bool loadWorkspace = true) override;

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
     * @return Preview image
     */
    QImage getWorkspacePreview(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const override;

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

private:

    /** Resets the manager and creates a new project */
    void createProject();

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
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
    QScopedPointer<mv::Project>         _project;                           /** Current project */
    mv::gui::TriggerAction              _newBlankProjectAction;             /** Action for creating a blank project (without view plugins and data) */
    mv::gui::TriggerAction              _newProjectFromWorkspaceAction;     /** Action for creating a new project from a workspace */
    mv::gui::TriggerAction              _openProjectAction;                 /** Action for opening a project */
    mv::gui::TriggerAction              _importProjectAction;               /** Action for importing a project */
    mv::gui::TriggerAction              _saveProjectAction;                 /** Action for saving a project */
    mv::gui::TriggerAction              _saveProjectAsAction;               /** Action for saving a project under a new name */
    mv::gui::TriggerAction              _editProjectSettingsAction;         /** Action for triggering the project settings dialog */
    mv::gui::RecentFilesAction          _recentProjectsAction;              /** Menu for loading recent projects */
    QMenu                               _newProjectMenu;                    /** Menu for creating a new project */
    QMenu                               _importDataMenu;                    /** Menu for importing data */
    mv::gui::TriggerAction              _publishAction;                     /** Action for publishing the project to an end-user */
    mv::gui::TriggerAction              _pluginManagerAction;               /** Action for showing the loaded plugins dialog */
    mv::gui::ToggleAction               _showStartPageAction;               /** Action for toggling the start page */
    mv::gui::TriggerAction              _backToProjectAction;               /** Action for going back to the project */
};

}