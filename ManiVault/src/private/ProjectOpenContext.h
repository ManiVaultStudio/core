// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WorkflowContextBase.h>

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;

/**
 * @brief Workflow context for project opening workflow
 *
 * This context encapsulates all relevant information and state for the project opening workflow, including file paths and temporary directory management.
 *
 * @author Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ProjectOpenContext : public WorkflowContextBase
{
public:

    /**
     * @brief Construct a new ProjectOpenContext with the specified project file path.
     * @param filePath Location on disk where the project should be saved
     */
    explicit ProjectOpenContext(const QString& filePath) :
		_filePath(filePath),
        _temporaryDirectory(new QTemporaryDir(QDir::cleanPath(mv::Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject")))
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
     * @brief Get project meta JSON file path
     * @return Location on disk where the project meta JSON file resides
     */
    QString getMetaJsonPath() const {
        QMutexLocker locker(&_mutex);
        return _metaJsonPath;
    }

    /**
     * @brief Set project meta JSON file path
     * @param path Location on disk where the project meta JSON file resides
     */
    void setMetaJsonPath(const QString& path) {
        QMutexLocker locker(&_mutex);
        _metaJsonPath = path;
    }

    /**
     * @brief Get temporary directory for saving the project
     * @return Unique pointer to a QTemporaryDir instance representing the temporary directory for saving the project
     */
    UniqueTemporaryDir getTemporaryDirectory() const {
        QMutexLocker locker(&_mutex);
        return std::make_unique<QTemporaryDir>(_temporaryDirectory->path());
    }

    /**
     * @brief Get temporary directory path for saving the project
     * @return Location on disk where the temporary directory for saving the project resides
     */
    QString getTemporaryDirectoryPath() const {
        QMutexLocker locker(&_mutex);
        return _temporaryDirectory->path();
    }

    /**
     * @brief Get project map for storing project data during the project opening process (e.g. to be used for passing data between workflow stages)
     * @return QVariantMap for storing project data during the project opening process (e.g. to be used for passing data between workflow stages)
     */
    QVariantMap getProjectMap() const {
        QMutexLocker locker(&_mutex);
        return _projectMap;
    }

    /**
     * @brief Set project map for storing project data during the project opening process (e.g. to be used for passing data between workflow stages)
     * @param projectMap QVariantMap for storing project data during the project opening process (e.g. to be used for passing data between workflow stages)
     */
    void setProjectMap(const QVariantMap& projectMap) {
        QMutexLocker locker(&_mutex);
        _projectMap = projectMap;
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
    mutable QMutex      _mutex;                 /** Mutex for synchronizing access to the context */
    QString             _filePath;              /** Location on disk where the project should be saved */
    QString             _workspaceJsonPath;     /** Location on disk where the workspace JSON file resides */
    QString             _projectJsonPath;       /** Location on disk where the project JSON file resides */
    QString             _metaJsonPath;          /** Location on disk where the project meta JSON file resides */
    UniqueTemporaryDir  _temporaryDirectory;    /** Unique pointer to a QTemporaryDir instance representing the temporary directory for saving the project */
    QVariantMap         _projectMap;            /** QVariantMap for storing project data during the project opening process (e.g. to be used for passing data between workflow stages) */
    QString             _errorMessage;          /** Error message string for storing any error that occurs during the project saving process */
};
