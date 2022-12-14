#pragma once

#include "Project.h"

#include <QObject>

namespace hdps {

/**
 * Abstract project manager class
 *
 * Base abstract plugin manager class for managing projects.
 *
 * @author Thomas Kroes
 */
class AbstractProjectManager : public QObject
{
    Q_OBJECT

public:

    /** Creates a new project */
    virtual void newProject() = 0;

    /**
     * Load a project from disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    virtual void loadProject(QString projectFilePath /*= ""*/) = 0;

    /**
     * Save a project to disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    virtual void saveProject(QString projectFilePath /*= ""*/) = 0;

signals:

    /**
     * Signals that a new project is created
     * @param project Reference to the created project
     */
    void projectCreated(const Project& project);

    /**
     * Signals that a project is destroyed
     * @param projectId Globally unique identifier of the project that is destroyed
     */
    void projectDestroyed(const QString& projectId);

    /**
     * Signals that a project is about to be loaded
     * @param project Reference to the project that is about to be loaded
     */
    void projectAboutToBeLoaded(const Project& project);

    /**
     * Signals that a project is loaded
     * @param project Reference to the project that is loaded
     */
    void projectLoaded(const Project& project);

    /**
     * Signals that a project is saved
     * @param project Reference to the saved project
     */
    void projectSaved(const Project& project);

    /**
     * Signals that a project is about to be destroyed
     * @param project Reference to the project that is about to be destroyed
     */
    void projectAboutToBeDestroyed(const Project& project);
};

}


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