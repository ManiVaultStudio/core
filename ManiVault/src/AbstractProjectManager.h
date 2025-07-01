// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Application.h"
#include "Project.h"
#include "ProjectSerializationTask.h"

#include "actions/TriggerAction.h"
#include "actions/RecentFilesAction.h"
#include "actions/ToggleAction.h"

#include <QObject>
#include <QMenu>
#include <QTemporaryDir>

#include "models/ProjectDatabaseTreeModel.h"

namespace mv {

class ProjectCenterModel;

/**
 * Abstract project manager class
 *
 * Base abstract plugin manager class for managing projects.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractProjectManager : public AbstractManager
{
    Q_OBJECT

public:

    /** The project manager is currently */
    enum class State {
        Idle,                   /** Not doing anything */
        OpeningProject,         /** Opening a project */
        ImportingProject,       /** Importing a project */
        SavingProject,          /** Saving a project */
        PublishingProject       /** Publishing a project */
    };

    /** Temporary directory in which temporary files are saved during: */
    enum class TemporaryDirType {
        Open,       /** Opening of a project */
        Save,       /** Saving of a project */
        Publish     /** Publishing of a project */
    };

    /**
     * Get temporary dir type name
     * @param temporaryDirType Temporary directory type
     * @return Temporary directory type name for \p temporaryDirType
     */
    static QString getTemporaryDirTypeName(const TemporaryDirType& temporaryDirType) {
        static QMap<TemporaryDirType, QString> temporaryDirTypeNames{
            { TemporaryDirType::Open, "Open" },
            { TemporaryDirType::Save, "Save" },
            { TemporaryDirType::Publish, "Publish" }
        };
        
        return temporaryDirTypeNames[temporaryDirType];
    };

    /** Set state for the duration of the enveloping scope, reverts to idle when the object gets out of scope */
    class CORE_EXPORT ScopedState {
    public:

        /**
         * Construct with initialization state
         * @param projectManager Pointer to project manager for which to set the state
         * @param state State at scope begin (reverts to idle when the object gets out of scope)
         */
        ScopedState(AbstractProjectManager* projectManager, const State& state) :
            _projectManager(projectManager),
            _state(state)
        {
            Q_ASSERT(_projectManager != nullptr);

            _projectManager->setState(state);
        }

        /** Revert to idle when object goes out of scope */
        ~ScopedState()
        {
            _projectManager->setState(State::Idle);
        }

    private:
        AbstractProjectManager* _projectManager;    /** Pointer to project manager for which to set the state */
        State                   _state;             /** State during the scope */
    };

    /** Task for monitoring the progress of: */
    enum class ProjectSerializationTaskType {
        ProjectSerialization,
        ProjectDataSerialization,
        ProjectWorkspaceSerialization
    };

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractProjectManager(QObject* parent) :
        AbstractManager(parent, "Project"),
        _state(State::Idle),
        _projectSerializationTask(this, "Project serialization")
    {
    }

    /**
     * Creates a new project, possibly also loading a workspace located at \p workspaceFilePath
     * @param workspaceFilePath File path of the workspace to load (will not attempt to load workspace if empty)
     */
    virtual void newProject(const QString& workspaceFilePath = "") = 0;

    /**
     * Creates a new project with default system view plugins (either on the left or right, other alignments are ignored)
     * @param alignment Alignment of the default plugins
     * @param logging Whether to add a logging view at the bottom
     */
    virtual void newProject(const Qt::AlignmentFlag& alignment, bool logging = false) = 0;

    /** Creates a new blank project (without any view plugins or data) */
    virtual void newBlankProject() = 0;

    /**
     * Open project from \p filePath
     * @param filePath File path of the project (choose file path when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied by the project
     */
    virtual void openProject(QString filePath = "", bool importDataOnly = false, bool loadWorkspace = true) = 0;

    /**
     * Download project from \p url, store it in \p targetDir and open it
     * @param url URL of the project
     * @param targetDirectory Directory where the project is stored (temporary directory when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied by the project
     */
    virtual void openProject(QUrl url, const QString& targetDirectory = "", bool importDataOnly = false, bool loadWorkspace = true) = 0;

    /**
     * Import project from \p filePath (only import the data)
     * @param filePath File path of the project (choose file path when empty)
     */
    virtual void importProject(QString filePath = "") = 0;

    /**
     * Save a project to \p filePath
     * @param filePath File path of the project (choose file path when empty)
     * @param password Encryption password
     */
    virtual void saveProject(QString filePath = "", const QString& password = "") = 0;

    /** Save project to different file (user is prompted to choose the file location) */
    virtual void saveProjectAs() = 0;

    /**
     * Publish project to \p filePath
     * @param filePath File path of the published project
     */
    virtual void publishProject(QString filePath = "") = 0;

    /**
     * Get whether a project exists
     * @return Boolean determining whether a project exists
     */
    virtual bool hasProject() const = 0;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    virtual const Project* getCurrentProject() const = 0;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    virtual Project* getCurrentProject() = 0;

    /**
     * Get project database tree model
     * @return Reference to the project database tree model
     */
    virtual const ProjectDatabaseTreeModel& getProjectDatabaseTreeModel() const = 0;

public: // Temporary directories

    /**
     * Get path to the temporary directory of \p temporaryDirType
     * Throws an exception if not found and returns the path to the application temporary directory as a fall-back
     * @param temporaryDirType Type of temporary directory, see AbstractProjectManager#TemporaryDirType
     * @return Path of the temporary directory
     */
    QString getTemporaryDirPath(const TemporaryDirType& temporaryDirType) const {
        if (_temporaryDirPaths.contains(temporaryDirType)) {
            return _temporaryDirPaths[temporaryDirType];
        }
        else {
            throw std::runtime_error(QString("Temporary directory path for project %1 does not exist").arg(getTemporaryDirTypeName(temporaryDirType).toLower()).toStdString());

            return Application::current()->getTemporaryDir().path();
        }
    }

protected: // Temporary directories

    /**
     * Set path to the temporary directory of \p temporaryDirType to \p temporaryDirPath
     * @param temporaryDirType Type of temporary directory, see AbstractProjectManager#TemporaryDirType
     * @param temporaryDirPath Path of the temporary directory
     */
    void setTemporaryDirPath(const TemporaryDirType& temporaryDirType, const QString& temporaryDirPath) {
        _temporaryDirPaths[temporaryDirType] = temporaryDirPath;
    }

    /**
     * Unset path to the temporary directory of \p temporaryDirType
     * @param temporaryDirType Type of temporary directory, see AbstractProjectManager#TemporaryDirType
     */
    void unsetTemporaryDirPath(const TemporaryDirType& temporaryDirType) {
        _temporaryDirPaths.remove(temporaryDirType);
    }

public:

    /**
     * Extract \p filePath from compressed ManiVault project in \p maniVaultFilePath
     * @param maniVaultFilePath File path of the compressed ManiVault file
     * @param temporaryDir Temporary directory where the extracted file resides
     * @param filePath Relative file path of the file that needs to be extracted
     * @return File path of the extracted file, empty string if extraction failed
     */
    virtual QString extractFileFromManiVaultProject(const QString& maniVaultFilePath, const QTemporaryDir& temporaryDir, const QString& filePath) = 0;

    /**
     * Get preview image of the project workspace
     * @param projectFilePath Path of the project file
     * @return Preview image
     */
    virtual QImage getWorkspacePreview(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const = 0;

    /** Get task for project serialization */
    ProjectSerializationTask& getProjectSerializationTask() {
        auto application = Application::current();

        if (application->getStartupTask().isRunning())
            return application->getStartupTask().getLoadProjectTask();

        return _projectSerializationTask;
    }

public: // Menus

    /**
     * Get new project menu
     * @return Pointer to new project menu
     */
    virtual QMenu& getNewProjectMenu() = 0;

    /**
     * Get import data menu
     * @return Pointer to import data menu
     */
    virtual QMenu& getImportDataMenu() = 0;

public: // State

    /**
     * Get the state of the project manager
     * @return State of the project manager
     */
    State getState() const {
        return _state;
    }

    /**
     * Set the state of the serializable object to \p state
     * @param state State of the serializable object
     */
    void setState(const State& state) {
        if (state == _state)
            return;

        _state = state;

        emit stateChanged(_state);
    }

    /**
     * Get whether the project manager is opening a project
     * @return Boolean determining whether the project manager is opening a project
     */
    bool isOpeningProject() const {
        return _state == State::OpeningProject;
    }

    /**
     * Get whether the project manager is importing a project
     * @return Boolean determining whether the project manager is importing a project
     */
    bool isImportingProject() const {
        return _state == State::ImportingProject;
    }

    /**
     * Get whether the project manager is saving a project
     * @return Boolean determining whether the project manager is saving a project
     */
    bool isSavingProject() const {
        return _state == State::SavingProject;
    }

    /**
     * Get whether the project manager is publishing a project
     * @return Boolean determining whether the project manager is publishing a project
     */
    bool isPublishingProject() const {
        return _state == State::PublishingProject;
    }

public: // Action getters

    virtual gui::TriggerAction& getNewBlankProjectAction() = 0;
    virtual gui::TriggerAction& getNewProjectFromWorkspaceAction() = 0;
    virtual gui::TriggerAction& getOpenProjectAction() = 0;
    virtual gui::TriggerAction& getImportProjectAction() = 0;
    virtual gui::TriggerAction& getSaveProjectAction() = 0;
    virtual gui::TriggerAction& getSaveProjectAsAction() = 0;
    virtual gui::TriggerAction& getEditProjectSettingsAction() = 0;
    virtual gui::RecentFilesAction& getRecentProjectsAction() = 0;
    virtual gui::TriggerAction& getPublishAction() = 0;
    virtual gui::TriggerAction& getPluginManagerAction() = 0;
    virtual gui::ToggleAction& getShowStartPageAction() = 0;
    virtual gui::TriggerAction& getBackToProjectAction() = 0;

signals:

    /** Signals that a new project is about to be created */
    void projectAboutToBeCreated();

    /**
     * Signals that \p project is created
     * @param project Reference to the created project
     */
    void projectCreated(const mv::Project& project);

    /**
     * Signals that project with \p projectId is destroyed
     * @param projectId Globally unique identifier of the project that is destroyed
     */
    void projectDestroyed(const QString& projectId);

    /**
     * Signals that \p project is about to be opened
     * @param project Reference to the project that is about to be opened
     */
    void projectAboutToBeOpened(const mv::Project& project);

    /**
     * Signals that \p project is opened
     * @param project Reference to the project that is opened
     */
    void projectOpened(const mv::Project& project);

    /**
     * Signals that a project is about to be imported from \p filePath
     * @param filePath Path of the project file which is about to be imported
     */
    void projectAboutToBeImported(const QString& filePath);

    /**
     * Signals that a project is imported from \p filePath
     * @param filePath Path of the project file which is imported
     */
    void projectImported(const QString& filePath);

    /**
     * Signals that \p project is about to be saved
     * @param project Reference to the project that is about to be saved
     */
    void projectAboutToBeSaved(const mv::Project& project);

    /**
     * Signals that \p project is saved
     * @param project Reference to the saved project
     */
    void projectSaved(const mv::Project& project);

    /**
     * Signals that \p project is about to be published
     * @param project Reference to the project that is about to be published
     */
    void projectAboutToBePublished(const mv::Project& project);

    /**
     * Signals that \p project is published
     * @param project Reference to the published project
     */
    void projectPublished(const mv::Project& project);

    /**
     * Signals that \p project is about to be destroyed
     * @param project Reference to the project that is about to be destroyed
     */
    void projectAboutToBeDestroyed(const mv::Project& project);

    /**
     * Signals that the state of the project manager changed to \p state
     * @param state State of the project manager
     */
    void stateChanged(const State& state);

private:
    State                               _state;                         /** Determines the state of the project manager */
    ProjectSerializationTask            _projectSerializationTask;      /** Task for project serialization */
    QMap<TemporaryDirType, QString>     _temporaryDirPaths;             /** Temporary directories for file open/save etc. */
};

}
