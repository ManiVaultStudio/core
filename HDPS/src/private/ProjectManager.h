#pragma once

#include "Project.h"

#include <QObject>
#include <QPointer>

/**
 * Project class
 *
 * Class for managing the creation/loading/saving of projects and contains a single project instance.
 *
 * @author Thomas Kroes
 */
class ProjectManager final : public QObject
{
    Q_OBJECT

public:

    /**
     * Creates a new project manager from with \p parent object
     * @param parent Pointer to parent object
     */
    ProjectManager(QObject* parent = nullptr);

    /** Creates a new project */
    void newProject();

    /**
     * Load a project from disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    void loadProject(QString projectFilePath /*= ""*/);

    /**
     * Save a project to disk
     * @param projectFilePath File path of the existing project (choose file path when empty)
     */
    void saveProject(QString projectFilePath /*= ""*/);

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

private:
    QPointer<Project>   _project;       /** Current project */
};
