#pragma once

#include "AbstractManager.h"
#include "Project.h"

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

    /**
     * Construct project manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractProjectManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Project")
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
     * @return File path of the extracted project.json file
     */
    virtual QString extractProjectFileFromHdpsFile(const QString& hdpsFilePath, QTemporaryDir& temporaryDir) = 0;

    /**
     * Get preview image of the project
     * @param projectFilePath Path of the project file
     * @return Preview image
     */
    virtual QImage getPreviewImage(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const = 0;

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
     * Signals that \p project is about to be loaded
     * @param project Reference to the project that is about to be loaded
     */
    void projectAboutToBeLoaded(const hdps::Project& project);

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
};

}
