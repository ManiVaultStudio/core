#pragma once

#include "AbstractManager.h"
#include "Project.h"

#include "actions/TriggerAction.h"
#include "actions/ToggleAction.h"

#include <QObject>
#include <QMenu>

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

    /** Creates a new project */
    virtual void newProject() = 0;

    /**
     * Load a project from disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    virtual void loadProject(QString projectFilePath = "") = 0;

    /**
     * Save a project to disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    virtual void saveProject(QString projectFilePath = "") = 0;

    /** Save project to different file (use is prompted to choose the file location) */
    virtual void saveProjectAs() = 0;

    /**
     * Get current project
     * @return Pointer to current project (nullptr if no project is loaded)
     */
    virtual Project* getCurrentProject() = 0;

public: // Menus

    /**
     * Get recent projects menu
     * @return Pointer to recent projects menu
     */
    virtual QMenu* getRecentProjectsMenu() = 0;

    /**
     * Get import data menu
     * @return Pointer to import data menu
     */
    virtual QMenu* getImportDataMenu() = 0;


public: // Action getters

    virtual hdps::gui::TriggerAction& getNewProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getOpenProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getSaveProjectAction() = 0;
    virtual hdps::gui::TriggerAction& getSaveProjectAsAction() = 0;
    virtual hdps::gui::ToggleAction& getShowStartPageAction() = 0;

signals:

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
     * Signals that \p project is loaded
     * @param project Reference to the project that is loaded
     */
    void projectLoaded(const hdps::Project& project);

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
     * Signals that \p project is about to be destroyed
     * @param project Reference to the project that is about to be destroyed
     */
    void projectAboutToBeDestroyed(const hdps::Project& project);
};

}

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

//QString Application::getCurrentProjectFilePath() const
//{
//    return _currentProjectFilePath;
//}
//
//void Application::setCurrentProjectFilePath(const QString& currentProjectFilePath)
//{
//    if (currentProjectFilePath == _currentProjectFilePath)
//        return;
//
//    _currentProjectFilePath = currentProjectFilePath;
//
//    emit currentProjectFilePathChanged(_currentProjectFilePath);
//}
//
//void Application::addRecentProjectFilePath(const QString& recentProjectFilePath)
//{
//    auto recentProjects = getSetting("Projects/Recent", QVariantList()).toList();
//
//    QVariantMap recentProject{
//        { "FilePath", recentProjectFilePath },
//        { "DateTime", QDateTime::currentDateTime() }
//    };
//
//    for (auto recentProject : recentProjects)
//        if (recentProject.toMap()["FilePath"].toString() == recentProjectFilePath)
//            recentProjects.removeOne(recentProject);
//
//    recentProjects.insert(0, recentProject);
//
//    setSetting("Projects/Recent", recentProjects);
//}
//
//Logger& Application::getLogger()
//{
//    return current()->_logger;
//}
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