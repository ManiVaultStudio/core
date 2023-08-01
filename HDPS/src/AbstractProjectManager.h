// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"
#include "Project.h"
#include "FileIOTask.h"

#include "actions/TriggerAction.h"
#include "actions/RecentFilesAction.h"
#include "actions/ToggleAction.h"

#include <QObject>
#include <QMenu>
#include <QTemporaryDir>

namespace hdps {

/**
 * Abstract project manager class
 *
 * Base abstract plugin manager class for managing projects.
 *
 * @author Thomas Kroes
 */
class AbstractProjectManager : public AbstractManager
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

    /** Set state for the duration of the enveloping scope, reverts to idle when the object gets out of scope */
    class ScopedState {
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

public:

    /**
     * Construct project manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractProjectManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Project"),
        _state(State::Idle),
        _fileIOTask(nullptr)
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
    virtual void newProject(const Qt::AlignmentFlag& defaultPluginsAlignment, bool logging = false) = 0;

    /** Creates a new blank project (without any view plugins or data) */
    virtual void newBlankProject() = 0;

    /**
     * Open project from \p filePath
     * @param filePath File path of the project (choose file path when empty)
     * @param importDataOnly Whether to only import the data from the project
     * @param loadWorkspace Whether to load the workspace which is accompanied with the project
     */
    virtual void openProject(QString filePath = "", bool importDataOnly = true, bool loadWorkspace = true) = 0;

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
     * Extract the project JSON file (project.json) from a compressed HDPS file (*.hdps)
     * @param hdpsFilePath File path of the compressed HDPS file (*.hdps)
     * @param temporaryDir Temporary directory to store the project.json file
     * @param filePath Relative file path of the file that needs to be extracted
     * @return File path of the extracted project.json file, empty string if extraction failed
     */
    virtual QString extractFileFromHdpsFile(const QString& hdpsFilePath, QTemporaryDir& temporaryDir, const QString& filePath) = 0;

    /**
     * Get preview image of the project
     * @param projectFilePath Path of the project file
     * @return Preview image
     */
    virtual QImage getPreviewImage(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const = 0;

    /**
     * Get file IO task
     * @return File IO task
     */
    virtual FileIOTask* getFileIOTask() final {
        if (_fileIOTask == nullptr) {
            _fileIOTask = new FileIOTask(this, "Project File IO");

            _fileIOTask->setProgressMode(Task::ProgressMode::Subtasks);
        }

        return _fileIOTask;
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
    virtual State getState() const final {
        return _state;
    }

    /**
     * Set the state of the serializable object to \p state
     * @param state State of the serializable object
     */
    virtual void setState(const State& state) final {
        if (state == _state)
            return;

        _state = state;

        emit stateChanged(_state);
    }

    /**
     * Get whether the project manager is opening a project
     * @return Boolean determining whether the project manager is opening a project
     */
    virtual bool isOpeningProject() const final {
        return _state == State::OpeningProject;
    }

    /**
     * Get whether the project manager is importing a project
     * @return Boolean determining whether the project manager is importing a project
     */
    virtual bool isImportingProject() const final {
        return _state == State::ImportingProject;
    }

    /**
     * Get whether the project manager is saving a project
     * @return Boolean determining whether the project manager is saving a project
     */
    virtual bool isSavingProject() const final {
        return _state == State::SavingProject;
    }

    /**
     * Get whether the project manager is publishing a project
     * @return Boolean determining whether the project manager is publishing a project
     */
    virtual bool isPublishingProject() const final {
        return _state == State::PublishingProject;
    }

public: // Action getters

    virtual hdps::gui::TriggerAction& getNewBlankProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getNewProjectFromWorkspaceAction() = 0;
    virtual hdps::gui::TriggerAction& getOpenProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getImportProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getSaveProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getSaveProjectAsAction() = 0;
    virtual hdps::gui::TriggerAction& getEditProjectSettingsAction() = 0;
    virtual hdps::gui::RecentFilesAction& getRecentProjectsAction() = 0;
    virtual hdps::gui::TriggerAction& getPublishAction() = 0;
    virtual hdps::gui::TriggerAction& getPluginManagerAction() = 0;
    virtual hdps::gui::ToggleAction& getShowStartPageAction() = 0;

signals:

    /** Signals that a new project is about to be created */
    void projectAboutToBeCreated();

    /**
     * Signals that \p project is created
     * @param project Reference to the created project
     */
    void projectCreated(const hdps::Project& project);

    /**
     * Signals that project with \p projectId is destroyed
     * @param projectId Globally unique identifier of the project that is destroyed
     */
    void projectDestroyed(const QString& projectId);

    /**
     * Signals that \p project is about to be opened
     * @param project Reference to the project that is about to be opened
     */
    void projectAboutToBeOpened(const hdps::Project& project);

    /**
     * Signals that \p project is opened
     * @param project Reference to the project that is opened
     */
    void projectOpened(const hdps::Project& project);

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
    void projectAboutToBeSaved(const hdps::Project& project);

    /**
     * Signals that \p project is saved
     * @param project Reference to the saved project
     */
    void projectSaved(const hdps::Project& project);

    /**
     * Signals that \p project is about to be published
     * @param project Reference to the project that is about to be published
     */
    void projectAboutToBePublished(const hdps::Project& project);

    /**
     * Signals that \p project is published
     * @param project Reference to the published project
     */
    void projectPublished(const hdps::Project& project);

    /**
     * Signals that \p project is about to be destroyed
     * @param project Reference to the project that is about to be destroyed
     */
    void projectAboutToBeDestroyed(const hdps::Project& project);

    /**
     * Signals that the state of the project manager changed to \p state
     * @param state State of the project manager
     */
    void stateChanged(const State& state);

private:
    State       _state;         /** Determines the state of the project manager */
    FileIOTask* _fileIOTask;    /** Task for reporting file IO operations */
};

}
