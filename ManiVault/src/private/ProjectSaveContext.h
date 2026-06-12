// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <workflow/WorkflowContextBase.h>

#include "Application.h"
#include "Archiver.h"

using UniqueTemporaryDir = std::unique_ptr<QTemporaryDir>;

/**
 * @brief Workflow context for project saving workflow
 *
 * This context encapsulates all necessary information and resources required for the project saving workflow, including file paths for the project and workspace JSON files, a temporary directory for intermediate storage during the saving process, and an archiver for handling the project archive.
 *
 * Thread safety is ensured through the use of a mutex to synchronize access to the context's data members, allowing safe concurrent access from multiple threads during the project saving workflow.
 *
 * @author Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class ProjectSaveContext : public mv::workflow::WorkflowContextBase
{
public:

    /**
     * @brief Construct project save context with project file path
     * @param filePath Location on disk where the project should be saved
     */
    explicit ProjectSaveContext(const QString& filePath) :
		_filePath(filePath),
		_temporaryDirectory(new QTemporaryDir(QDir::cleanPath(mv::Application::current()->getTemporaryDir().path() + QDir::separator() + "SaveProject")))
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
     * @brief Get archiver for handling the project archive
     * @return Reference to an Archiver instance for handling the project archive
     */
	mv::util::Archiver& getArchiver() {
		QMutexLocker locker(&_mutex);
		return _archiver;
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
    QString             _filePath;              /** Path to the project file */
    QString             _workspaceJsonPath;     /** Path to the workspace JSON file */
    QString             _projectJsonPath;       /** Path to the project JSON file */
    QString             _metaJsonPath;          /** Path to the project meta JSON file */
    UniqueTemporaryDir  _temporaryDirectory;    /** Temporary directory for saving the project */
    mv::util::Archiver  _archiver;              /** Archiver for handling the project archive */
    QString             _errorMessage;          /** Error message string for storing any error that occurs during the project saving process */
};
