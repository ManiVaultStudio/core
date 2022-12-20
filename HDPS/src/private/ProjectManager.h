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

    /** Creates a new project */
    void newProject() override;

    /**
     * Load project from \p filePath
     * @param filePath File path of the existing project (choose file path when empty)
     */
    void loadProject(QString filePath = "") override;

    /**
     * Save project to \p filePath (uses file path of loaded project when empty)
     * @param filePath File path of the existing project (choose file path when empty)
     */
    void saveProject(QString filePath = "") override;

    /** Save project to different file (use is prompted to choose the file location) */
    void saveProjectAs() override;

    /**
     * Get whether a project exists
     * @return Boolean determining whether a project exists
     */
    bool hasProject() const override;

    /**
     * Get current project
     * @return Pointer to project (nullptr if no project is loaded)
     */
    const hdps::Project* getProject() const override;

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
     * Get import data menu
     * @return Pointer to import data menu
     */
    QMenu* getImportDataMenu() override;

private:

    /** Resets the core and creates a new project */
    void createProject();

public: // Action getters

    hdps::gui::TriggerAction& getNewProjectAction() override { return _newProjectAction; }
    hdps::gui::TriggerAction& getOpenProjectAction() override { return _openProjectAction; }
    hdps::gui::TriggerAction& getSaveProjectAction() override { return _saveProjectAction; }
    hdps::gui::TriggerAction& getSaveProjectAsAction() override { return _saveProjectAsAction; }
    hdps::gui::RecentFilesAction& getRecentProjectsAction() override { return _recentProjectsAction; }
    hdps::gui::TriggerAction& getPublishAction() override { return _publishAction; }
    hdps::gui::TriggerAction& getPluginManagerAction() override { return _pluginManagerAction; }
    hdps::gui::ToggleAction& getShowStartPageAction() override { return _showStartPageAction; }

private:
    QScopedPointer<hdps::Project>       _project;                   /** Current project */
    hdps::gui::TriggerAction            _newProjectAction;          /** Action for creating a new project */
    hdps::gui::TriggerAction            _openProjectAction;         /** Action for opening a project */
    hdps::gui::TriggerAction            _saveProjectAction;         /** Action for saving a project */
    hdps::gui::TriggerAction            _saveProjectAsAction;       /** Action for saving a project under a new name */
    hdps::gui::RecentFilesAction        _recentProjectsAction;      /** Menu for loading recent projects */
    QMenu                               _importDataMenu;            /** Menu for importing data */
    hdps::gui::TriggerAction            _publishAction;             /** Action for publishing the project to an end-user */
    hdps::gui::TriggerAction            _pluginManagerAction;       /** Action for showing the loaded plugins dialog */
    hdps::gui::ToggleAction             _showStartPageAction;       /** Action for toggling the start page */

protected:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};
