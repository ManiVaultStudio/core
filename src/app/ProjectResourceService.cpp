// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ProjectResourceServiceProvider.h"

#include "Archiver.h"
#include "ProjectManager.h"
#include "ProjectResourceStagingService.h"

#include <AbstractProjectManager.h>
#include <Project.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTemporaryDir>
#include <QUuid>

#include <quazip/quazip.h>

#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace mv
{
namespace
{
constexpr auto resourceScheme = "project-resource://";
constexpr qint64 ioChunkSize = 1024 * 1024;

QString resourcePathFromReference(const ProjectResourceReference& reference)
{
    if (!reference.isValid())
        throw std::invalid_argument("Project resource reference is invalid");

    const auto path = reference.toString().mid(static_cast<qsizetype>(QString::fromLatin1(resourceScheme).size()));
    const auto normalizedPath = QDir::cleanPath(path);
    if (normalizedPath != path || path.startsWith(QLatin1Char('/')) || path.contains(QLatin1Char('\\')) || !path.startsWith(QStringLiteral("resources/")))
        throw std::invalid_argument("Project resource reference contains an invalid path");

    return normalizedPath;
}

std::uint64_t getArchiveEntrySize(const QString& archivePath, const QString& entryName)
{
    QuaZip zip(archivePath);
    if (!zip.open(QuaZip::mdUnzip))
        throw std::runtime_error(QString("Unable to open project archive: %1").arg(archivePath).toStdString());
    if (!zip.setCurrentFile(entryName))
        throw std::runtime_error(QString("Unable to locate project resource: %1").arg(entryName).toStdString());

    QuaZipFileInfo64 entryInfo;
    if (!zip.getCurrentFileInfo(&entryInfo))
        throw std::runtime_error(QString("Unable to inspect project resource: %1").arg(entryName).toStdString());

    zip.close();
    return static_cast<std::uint64_t>(entryInfo.uncompressedSize);
}

QByteArray readLocalRange(const QString& path, const std::uint64_t offset, const std::uint64_t size)
{
    if (offset > static_cast<std::uint64_t>(std::numeric_limits<qint64>::max()) || size > static_cast<std::uint64_t>(std::numeric_limits<qint64>::max()))
        throw std::out_of_range("Project resource range exceeds supported file sizes");

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly) || !file.seek(static_cast<qint64>(offset)))
        throw std::runtime_error(QString("Unable to read project resource: %1").arg(path).toStdString());

    const auto bytes = file.read(static_cast<qint64>(size));
    if (bytes.size() != static_cast<qsizetype>(size))
        throw std::runtime_error(QString("Unable to read project resource range: %1").arg(path).toStdString());

    return bytes;
}

QString copyToTemporaryFile(const QString& sourcePath, const std::uint64_t expectedSize, std::shared_ptr<QTemporaryDir>& directory)
{
    directory = std::make_shared<QTemporaryDir>(QDir(QDir::tempPath()).filePath(QStringLiteral("ManiVaultProjectResource-XXXXXX")));
    if (!directory->isValid())
        throw std::runtime_error("Unable to create a temporary directory for a project resource");

    const auto fileName = QFileInfo(sourcePath).fileName();
    const auto destinationPath = directory->filePath(fileName.isEmpty() ? QStringLiteral("resource") : fileName);
    QFile input(sourcePath);
    QSaveFile output(destinationPath);
    if (!input.open(QIODevice::ReadOnly) || !output.open(QIODevice::WriteOnly))
        throw std::runtime_error(QString("Unable to stage project resource: %1").arg(sourcePath).toStdString());

    std::uint64_t bytesWritten = 0;
    while (!input.atEnd())
    {
        const auto bytes = input.read(ioChunkSize);
        if (bytes.isEmpty() && !input.atEnd())
            throw std::runtime_error(QString("Failed while staging project resource: %1").arg(sourcePath).toStdString());
        if (!bytes.isEmpty() && output.write(bytes) != bytes.size())
            throw std::runtime_error(QString("Unable to write staged project resource: %1").arg(destinationPath).toStdString());
        bytesWritten += static_cast<std::uint64_t>(bytes.size());
    }

    if (bytesWritten != expectedSize || !output.commit())
        throw std::runtime_error(QString("Unable to finalize staged project resource: %1").arg(destinationPath).toStdString());

    return destinationPath;
}

class ProjectResourceHandleImplementation final : public ProjectResourceHandle
{
public:
    ProjectResourceHandleImplementation(QString archivePath, QString archiveEntry, QString localPath, const std::uint64_t resourceSize, const util::ProjectResourceStagingPolicy policy, std::shared_ptr<ProjectResourceStagingService> stagingService, std::shared_ptr<QTemporaryDir> stagingDirectory, std::shared_ptr<QTemporaryDir> localResourceDirectory) :
        _archivePath(std::move(archivePath)),
        _archiveEntry(std::move(archiveEntry)),
        _localPath(std::move(localPath)),
        _resourceSize(resourceSize),
        _policy(policy),
        _stagingService(std::move(stagingService)),
        _stagingDirectory(std::move(stagingDirectory)),
        _localResourceDirectory(std::move(localResourceDirectory))
    {
    }

    std::uint64_t size() const override
    {
        return _resourceSize;
    }

    QByteArray readRange(const std::uint64_t offset, const std::uint64_t size) const override
    {
        if (offset > _resourceSize || size > _resourceSize - offset)
            throw std::out_of_range("Requested range exceeds project resource");
        if (size == 0)
            return {};
        if (!_localPath.isEmpty())
            return readLocalRange(_localPath, offset, size);

        if (_policy != util::ProjectResourceStagingPolicy::ReadRangesOnDemand)
        {
            const auto stagedPath = _stagingService->resolvePath(_archiveEntry);
            if (!stagedPath.isEmpty())
                return readLocalRange(stagedPath, offset, size);
        }

        return util::Archiver::readZipEntryRangeToMemory(_archivePath, _archiveEntry, offset, size);
    }

    StagedProjectResourceFile stagedFile() const override
    {
        QString sourcePath = _localPath;
        if (sourcePath.isEmpty())
        {
            _stagingService->registerResources({ ProjectResourceStagingService::ResourceRequest{ _archiveEntry, util::ProjectResourceStagingPolicy::StageOnFirstRead } });
            sourcePath = _stagingService->resolvePath(_archiveEntry);
        }

        std::shared_ptr<QTemporaryDir> leaseDirectory;
        const auto stagedPath = copyToTemporaryFile(sourcePath, _resourceSize, leaseDirectory);
        return StagedProjectResourceFile(stagedPath, _resourceSize, std::shared_ptr<const void>(leaseDirectory, leaseDirectory.get()));
    }

private:
    QString                                 _archivePath; /**< Archive resolved when the resource handle was opened. */
    QString                                 _archiveEntry; /**< Project-relative archive entry. */
    QString                                 _localPath; /**< Independent local copy for an unsaved resource, when applicable. */
    std::uint64_t                           _resourceSize = 0; /**< Uncompressed resource size in bytes. */
    util::ProjectResourceStagingPolicy      _policy = util::ProjectResourceStagingPolicy::ReadRangesOnDemand; /**< Effective staging behavior. */
    std::shared_ptr<ProjectResourceStagingService> _stagingService; /**< Session service retained for on-demand access. */
    std::shared_ptr<QTemporaryDir>          _stagingDirectory; /**< Keeps this project's staging area alive while the handle exists. */
    std::shared_ptr<QTemporaryDir>          _localResourceDirectory; /**< Keeps a copy of an unsaved resource alive while the handle exists. */
};

class ProjectResourceServiceImplementation final : public ProjectResourceService
{
public:
    ProjectResourceServiceImplementation(Project& project, ProjectManager& projectManager, std::shared_ptr<ProjectResourceStagingService> stagingService, std::shared_ptr<QTemporaryDir> stagingDirectory) :
        _project(project),
        _projectManager(projectManager),
        _stagingService(std::move(stagingService)),
        _stagingDirectory(std::move(stagingDirectory))
    {
    }

    ProjectResourceReference addResource(QIODevice& source, const QString& resourceName) override
    {
        if (!source.isOpen() || !source.isReadable())
            throw std::invalid_argument("Project resource source must be open and readable");
        if (resourceName.isEmpty())
            throw std::invalid_argument("Project resource name cannot be empty");
        if (_projectManager.getCurrentProject() != &_project)
            throw std::logic_error("Project resources can only be added to the active project");
        if (_project.getReadOnlyAction().isChecked())
            throw std::logic_error("Project resources cannot be added to a read-only project");

        const auto fileName = QFileInfo(resourceName).fileName();
        if (fileName.isEmpty() || fileName == QStringLiteral(".") || fileName == QStringLiteral(".."))
            throw std::invalid_argument("Project resource name must include a file name");

        const auto resourcePath = QStringLiteral("resources/%1/%2").arg(QUuid::createUuid().toString(QUuid::WithoutBraces), fileName);
        const auto saveDirectory = _projectManager.getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save);
        const auto destinationPath = QDir(saveDirectory).filePath(resourcePath);
        if (!QDir().mkpath(QFileInfo(destinationPath).absolutePath()))
            throw std::runtime_error(QString("Unable to create project resource directory: %1").arg(destinationPath).toStdString());

        QSaveFile destination(destinationPath);
        if (!destination.open(QIODevice::WriteOnly))
            throw std::runtime_error(QString("Unable to create project resource: %1").arg(destinationPath).toStdString());

        while (!source.atEnd())
        {
            const auto bytes = source.read(ioChunkSize);
            if (bytes.isEmpty() && !source.atEnd())
                throw std::runtime_error(QString("Unable to read project resource source: %1").arg(resourceName).toStdString());
            if (!bytes.isEmpty() && destination.write(bytes) != bytes.size())
                throw std::runtime_error(QString("Unable to write project resource: %1").arg(destinationPath).toStdString());
        }

        if (!destination.commit())
            throw std::runtime_error(QString("Unable to finalize project resource: %1").arg(destinationPath).toStdString());

        return ProjectResourceReference(QString::fromLatin1(resourceScheme) + resourcePath);
    }

    ProjectResourceReference addResourceFile(const QString& sourceFilePath, const QString& resourceName) override
    {
        QFile source(sourceFilePath);
        if (!source.open(QIODevice::ReadOnly))
            throw std::runtime_error(QString("Unable to open project resource source file: %1").arg(sourceFilePath).toStdString());

        return addResource(source, resourceName.isEmpty() ? QFileInfo(sourceFilePath).fileName() : resourceName);
    }

    std::shared_ptr<ProjectResourceHandle> openResource(const ProjectResourceReference& reference, const util::ProjectResourceStagingPolicy stagingPolicy) const override
    {
        if (_projectManager.getCurrentProject() != &_project)
            throw std::logic_error("Project resources can only be opened from the active project");

        const auto resourcePath = resourcePathFromReference(reference);
        auto policy = stagingPolicy == util::ProjectResourceStagingPolicy::Inherit ? _stagingService->getEffectiveDefaultPolicy() : stagingPolicy;
        QString localPath;
        std::shared_ptr<QTemporaryDir> localResourceDirectory;
        std::uint64_t resourceSize = 0;

        try
        {
            const auto saveDirectory = _projectManager.getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Save);
            const auto savePath = QDir(saveDirectory).filePath(resourcePath);
            if (QFileInfo(savePath).isFile())
            {
                resourceSize = static_cast<std::uint64_t>(QFileInfo(savePath).size());
                localPath = copyToTemporaryFile(savePath, resourceSize, localResourceDirectory);
            }
        }
        catch (const std::runtime_error&)
        {
            // Resolve the reference from the saved archive when no save staging directory is active.
        }

        if (localPath.isEmpty())
        {
            resourceSize = getArchiveEntrySize(_project.getFilePath(), resourcePath);
            _stagingService->registerResources({ ProjectResourceStagingService::ResourceRequest{ resourcePath, policy } });
        }

        auto resource = std::make_shared<ProjectResourceHandleImplementation>(_project.getFilePath(), resourcePath, localPath, resourceSize, policy, _stagingService, _stagingDirectory, localResourceDirectory);
        if (localPath.isEmpty() && policy == util::ProjectResourceStagingPolicy::StageImmediately)
            static_cast<void>(_stagingService->resolvePath(resourcePath));
        return resource;
    }

private:
    Project& _project; /**< Project that owns this service. */
    ProjectManager& _projectManager; /**< Manager providing active-project and save-directory state. */
    std::shared_ptr<ProjectResourceStagingService> _stagingService; /**< Project-specific archive staging session. */
    std::shared_ptr<QTemporaryDir> _stagingDirectory; /**< Project staging directory lease. */
};
}

std::unique_ptr<ProjectResourceService> createProjectResourceService(Project& project, ProjectManager& projectManager, std::shared_ptr<ProjectResourceStagingService> stagingService, std::shared_ptr<QTemporaryDir> stagingDirectory)
{
    return std::make_unique<ProjectResourceServiceImplementation>(project, projectManager, std::move(stagingService), std::move(stagingDirectory));
}
}
