#pragma once

#include <AbstractProjectManager.h>

#include <QObject>
#include <QPointer>

/**
 * Project class
 *
 * Class for managing the creation/loading/saving of projects and contains a single project instance.
 *
 * @author Thomas Kroes
 */
class ProjectManager final : public hdps::AbstractProjectManager
{
    Q_OBJECT

public:

    /**
     * Creates a new project manager from with \p parent object
     * @param parent Pointer to parent object
     */
    ProjectManager(QObject* parent = nullptr);

    /** Perform manager startup initialization */
    void initalize() override;

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
     */
    void saveProject(QString filePath = "") override;

    /** Save project to different file (user is prompted to choose the file location) */
    void saveProjectAs() override;

    /**
     * Get whether a project exists
     * @return Boolean determining whether a project exists
     */
    bool hasProject() const override;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    const hdps::Project* getCurrentProject() const override;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    hdps::Project* getCurrentProject() override;

    /**
     * Extract the project JSON file (project.json) from a compressed HDPS file (*.hdps)
     * @param hdpsFilePath File path of the compressed HDPS file (*.hdps)
     * @param temporaryDir Temporary directory to store the project.json file
     * @return File path of the extracted project.json file
     */
    QString extractProjectFileFromHdpsFile(const QString& hdpsFilePath, QTemporaryDir& temporaryDir) override;

    /**
     * Get preview image of the project
     * @param projectFilePath Path of the project file
     * @return Preview image
     */
    QImage getPreviewImage(const QString& projectFilePath, const QSize& targetSize = QSize(500, 500)) const override;

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

    /** Resets the core and creates a new project */
    void createProject();

public: // Action getters

    hdps::gui::TriggerAction& getNewBlankProjectAction() override { return _newBlankProjectAction; }
    hdps::gui::TriggerAction& getNewProjectFromWorkspaceAction() override { return _newProjectFromWorkspaceAction; }
    hdps::gui::TriggerAction& getOpenProjectAction() override { return _openProjectAction; }
    hdps::gui::TriggerAction& getImportProjectAction() override { return _importProjectAction; };
    hdps::gui::TriggerAction& getSaveProjectAction() override { return _saveProjectAction; }
    hdps::gui::TriggerAction& getSaveProjectAsAction() override { return _saveProjectAsAction; }
    hdps::gui::TriggerAction& getEditProjectSettingsAction() override { return _editProjectSettingsAction; }
    hdps::gui::RecentFilesAction& getRecentProjectsAction() override { return _recentProjectsAction; }
    hdps::gui::TriggerAction& getPublishAction() override { return _publishAction; }
    hdps::gui::TriggerAction& getPluginManagerAction() override { return _pluginManagerAction; }
    hdps::gui::ToggleAction& getShowStartPageAction() override { return _showStartPageAction; }

private:
    QScopedPointer<hdps::Project>       _project;                           /** Current project */
    hdps::gui::TriggerAction            _newBlankProjectAction;             /** Action for creating a blank project (without view plugins and data) */
    hdps::gui::TriggerAction            _newProjectFromWorkspaceAction;     /** Action for creating a new project from a workspace */
    hdps::gui::TriggerAction            _openProjectAction;                 /** Action for opening a project */
    hdps::gui::TriggerAction            _importProjectAction;               /** Action for importing a project */
    hdps::gui::TriggerAction            _saveProjectAction;                 /** Action for saving a project */
    hdps::gui::TriggerAction            _saveProjectAsAction;               /** Action for saving a project under a new name */
    hdps::gui::TriggerAction            _editProjectSettingsAction;         /** Action for triggering the project settings dialog */
    hdps::gui::RecentFilesAction        _recentProjectsAction;              /** Menu for loading recent projects */
    QMenu                               _newProjectMenu;                    /** Menu for creating a new project */
    QMenu                               _importDataMenu;                    /** Menu for importing data */
    hdps::gui::TriggerAction            _publishAction;                     /** Action for publishing the project to an end-user */
    hdps::gui::TriggerAction            _pluginManagerAction;               /** Action for showing the loaded plugins dialog */
    hdps::gui::ToggleAction             _showStartPageAction;               /** Action for toggling the start page */
};
