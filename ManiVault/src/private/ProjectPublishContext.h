// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/WorkflowContextBase.h>

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;

/**
 * @brief Workflow context for project publishing workflow
 *
 * This class encapsulates the context information required for the project publishing workflow, including the project file path, temporary directory for saving the project, workspace JSON file path, project JSON file path, and error message string. It provides thread-safe access to these context variables using a mutex.
 *
 * @author Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ProjectPublishContext : public mv::workflow::WorkflowContextBase
{
public:

    /**
     * @brief Construct project publish context with project file path
     * @param filePath Location on disk where the project should be saved
     */
    explicit ProjectPublishContext(const QString& filePath) :
		_filePath(filePath),
        _temporaryDirectory(new QTemporaryDir(QDir::cleanPath(mv::Application::current()->getTemporaryDir().path() + QDir::separator() + "PublishProject")))
    {
    }

    /**
     * @brief Get project file path
     * @return Location on disk where the project should be saved
     */
    QString getFilePath() const {
        QMutexLocker locker(&_mutex);
        return _filePath;
    }

    /**
     * @brief Get temporary directory path for publishing the project
     * @return Location on disk of the temporary directory for publishing the project
     */
    QString getTemporaryDirectoryPath() const {
        QMutexLocker locker(&_mutex);
        return _temporaryDirectoryPath;
    }

    /**
     * @brief Get workspace JSON file path
     * @return Location on disk where the workspace JSON file resides
     */
    QString getWorkspaceJsonPath() const {
        QMutexLocker locker(&_mutex);
        return _workspaceJsonPath;
    }

    /**
     * @brief Set workspace JSON file path
     * @param path Location on disk where the workspace JSON file resides
     */
    void setWorkspaceJsonPath(const QString& path) {
        QMutexLocker locker(&_mutex);
        _workspaceJsonPath = path;
    }

    /**
     * @brief Get project JSON file path
     * @return Location on disk where the project JSON file resides
     */
    QString getProjectJsonPath() const {
        QMutexLocker locker(&_mutex);
        return _projectJsonPath;
    }

    /**
     * @brief Set project JSON file path
     * @param path Location on disk where the project JSON file resides
     */
    void setProjectJsonPath(const QString& path) {
        QMutexLocker locker(&_mutex);
        _projectJsonPath = path;
    }

    /**
     * @brief Get error message
     * @return Error message string
     */
    QString getErrorMessage() const {
        QMutexLocker locker(&_mutex);
        return _errorMessage;
    }

private:
    mutable QMutex      _mutex;                     /** Mutex for synchronizing access to the context */
    QString             _filePath;                  /** Location on disk where the published project should be saved */
    UniqueTemporaryDir  _temporaryDirectory;        /** Temporary directory for publishing the project */
    QString             _temporaryDirectoryPath;    /** Path to the temporary directory for publishing the project */
    QString             _workspaceJsonPath;         /** Path to the workspace JSON file */
    QString             _projectJsonPath;           /** Path to the project JSON file */
    QString             _errorMessage;              /** Error message string for storing any error that occurs during the project publishing process */
};
