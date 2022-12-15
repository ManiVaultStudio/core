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
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    hdps::Project* getCurrentProject() override;

public: // Recent projects

    /**
     * Get recent projects menu
     * @return Pointer to recent projects menu
     */
    QMenu* getRecentProjectsMenu() override;

private:

    /** Resets the core and creates a new project */
    void createProject();

    /** Update the contents of the recent projects menu */
    void updateRecentProjectsMenu();

    /** Add loaded project to recent projects in the settings */
    void addRecentProjectFilePath(const QString& filePath);

public: // Action getters

    hdps::gui::TriggerAction& getNewProjectAction() override { return _newProjectAction; }
    hdps::gui::TriggerAction& getOpenProjectAction() override { return _openProjectAction; }
    hdps::gui::TriggerAction& getSaveProjectAction() override { return _saveProjectAction; }
    hdps::gui::TriggerAction& getSaveProjectAsAction() override { return _saveProjectAsAction; }

private:
    QScopedPointer<hdps::Project>   _project;                   /** Current project */
    hdps::gui::TriggerAction        _newProjectAction;          /** Action for creating a new project */
    hdps::gui::TriggerAction        _openProjectAction;         /** Action for opening a project */
    hdps::gui::TriggerAction        _saveProjectAction;         /** Action for saving a project */
    hdps::gui::TriggerAction        _saveProjectAsAction;       /** Action for saving a project under a new name */
    QMenu                           _recentProjectsMenu;        /** Menu for loading recent projects */

    QString         _serializationTemporaryDirectory;       /** Temporary directory for serialization */
    bool            _serializationAborted;                  /** Whether (de)serialization was aborted */

protected:
    static constexpr bool           DEFAULT_ENABLE_COMPRESSION  = false;    /** No compression by default */
    static constexpr std::uint32_t  DEFAULT_COMPRESSION_LEVEL   = 2;        /** Default compression level*/
};


///**
// * Get current project file path
// * @return File path of the current project
// */
//virtual QString getCurrentProjectFilePath() const final;
//
///**
// * Set current project file path
// * @param currentProjectFilePath Current project file path
// */
//virtual void setCurrentProjectFilePath(const QString& currentProjectFilePath) final;
//
///**
// * Add recent project file path (adds the path to the settings so that users can select the project from a recent list)
// * @param recentProjectFilePath File path of the recent project
// */
//virtual void addRecentProjectFilePath(const QString& recentProjectFilePath) final;

///**
//     * Get serialization temporary directory
//     * @return Serialization temporary directory
//     */
//static QString getSerializationTemporaryDirectory();
//
///**
// * Get whether (de)serialization was aborted
// * @return Boolean indicating whether (de)serialization was aborted
// */
//static bool isSerializationAborted();

//signals:
//
//    /**
//     * Signals that the current project file changed
//     */
//    void currentProjectFilePathChanged(const QString& currentProjectFilePath);

//
//QString Application::getSerializationTemporaryDirectory()
//{
//    return current()->_serializationTemporaryDirectory;
//}
//
//bool Application::isSerializationAborted()
//{
//    return current()->_serializationAborted;
//}