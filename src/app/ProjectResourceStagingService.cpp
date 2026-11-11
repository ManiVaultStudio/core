// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ProjectResourceStagingService.h"

#include "Archiver.h"
#include "Task.h"

#include <workflow/WorkflowExecutionContext.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUuid>

#include <algorithm>
#include <chrono>
#include <exception>
#include <stdexcept>
#include <utility>

namespace mv
{

void ProjectResourceStagingService::configure(QString archivePath, QString stagingDirectory, const Policy policy)
{
    std::lock_guard lock(_mutex);
    _archivePath = std::move(archivePath);
    _stagingDirectory = QDir::cleanPath(std::move(stagingDirectory));
    _systemPolicy = policy;
    _projectPolicy = Policy::Inherit;
    _resources.clear();
}

void ProjectResourceStagingService::clear()
{
    std::lock_guard lock(_mutex);
    _archivePath.clear();
    _stagingDirectory.clear();
    _systemPolicy = Policy::ArchiveBacked;
    _projectPolicy = Policy::Inherit;
    _resources.clear();
}

void ProjectResourceStagingService::setProjectPolicy(const Policy policy)
{
    std::lock_guard lock(_mutex);
    _projectPolicy = policy;
}

ProjectResourceStagingService::Policy ProjectResourceStagingService::getEffectiveDefaultPolicy() const
{
    std::lock_guard lock(_mutex);
    return _projectPolicy == Policy::Inherit ? _systemPolicy : _projectPolicy;
}

QString ProjectResourceStagingService::validateArchiveEntry(const QString& archiveEntry)
{
    const auto trimmedEntry = archiveEntry.trimmed();
    if (trimmedEntry.isEmpty() || QDir::isAbsolutePath(trimmedEntry) || trimmedEntry.contains(QLatin1Char('\\')) || trimmedEntry.contains(QLatin1Char(':')))
        throw std::runtime_error(QString("Unsafe project resource archive path: %1").arg(archiveEntry).toStdString());

    const auto pathParts = trimmedEntry.split(QLatin1Char('/'), Qt::KeepEmptyParts);
    if (std::any_of(pathParts.cbegin(), pathParts.cend(), [](const QString& part) { return part.isEmpty() || part == QStringLiteral(".") || part == QStringLiteral(".."); }))
        throw std::runtime_error(QString("Unsafe project resource archive path: %1").arg(archiveEntry).toStdString());

    return trimmedEntry;
}

void ProjectResourceStagingService::registerResources(const QStringList& archiveEntries)
{
    QList<ResourceRequest> requests;
    requests.reserve(archiveEntries.size());
    for (const auto& entry : archiveEntries)
        requests.push_back({ entry, Policy::Inherit });
    registerResources(requests);
}

void ProjectResourceStagingService::registerResources(const QList<ResourceRequest>& requests)
{
    std::lock_guard lock(_mutex);
    const auto effectivePolicy = [this](const Policy requestedPolicy) {
        if (requestedPolicy != Policy::Inherit)
            return requestedPolicy;
        return _projectPolicy != Policy::Inherit ? _projectPolicy : _systemPolicy;
    };
    const auto policyRank = [](const Policy policy) {
        switch (policy)
        {
            case Policy::ArchiveBacked: return 0;
            case Policy::OnDemand: return 1;
            case Policy::BeforeLoad: return 2;
            case Policy::Inherit: return -1;
        }
        return -1;
    };
    for (const auto& request : requests)
    {
        const auto safeEntry = validateArchiveEntry(request.archiveEntry);
        const auto resolvedPolicy = effectivePolicy(request.policy);
        if (!_resources.contains(safeEntry))
        {
            auto state = std::make_shared<ResourceState>();
            state->policy = resolvedPolicy;
            _resources.insert(safeEntry, std::move(state));
        }
        else if (policyRank(resolvedPolicy) > policyRank(_resources.value(safeEntry)->policy))
            _resources.value(safeEntry)->policy = resolvedPolicy;
    }
}

workflow::UniqueWorkflowPlan ProjectResourceStagingService::createEagerStagingWorkflow(const std::size_t maxConcurrency)
{
    auto plan = std::make_unique<workflow::WorkflowPlan>(QStringLiteral("Stage project resources"));
    QStringList archiveEntries;
    {
        std::lock_guard lock(_mutex);
        for (auto it = _resources.cbegin(); it != _resources.cend(); ++it)
            if (it.value()->policy == Policy::BeforeLoad)
                archiveEntries.push_back(it.key());
    }

    if (archiveEntries.isEmpty())
    {
        plan->addSequentialStage("No project resources to stage", [](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext& executionContext) {
            executionContext->setProgress(1.0);
        });
        return plan;
    }

    workflow::WorkflowPlan::Jobs jobs;
    jobs.reserve(static_cast<std::size_t>(archiveEntries.size()));
    for (const auto& archiveEntry : archiveEntries)
    {
        jobs.emplace_back(QStringLiteral("Stage %1").arg(archiveEntry), [this, archiveEntry](const workflow::WorkflowPlan::Job&, const workflow::SharedWorkflowExecutionContext& executionContext) {
            const auto task = executionContext->getTask();
            const auto cancellationCheck = [task]() {
                return task && (task->isAboutToBeAborted() || task->isAborting() || task->isAborted());
            };
            const auto progress = [executionContext](const std::uint64_t completed, const std::uint64_t total) {
                executionContext->setProgress(total == 0 ? 1.0 : static_cast<double>(completed) / static_cast<double>(total));
            };
            const auto stagedPath = stage(archiveEntry, cancellationCheck, progress);
            static_cast<void>(stagedPath);
        });
    }

    plan->addBatchedParallelStage(QStringLiteral("Extract packed point-data files"), std::move(jobs), std::max<std::size_t>(1, maxConcurrency));
    return plan;
}

QString ProjectResourceStagingService::resolvePath(const QString& archiveEntry)
{
    std::shared_ptr<ResourceState> resourceState;
    {
        std::lock_guard lock(_mutex);
        resourceState = _resources.value(validateArchiveEntry(archiveEntry));
    }

    if (!resourceState)
        throw std::runtime_error(QString("Project resource was not registered for staging: %1").arg(archiveEntry).toStdString());

    if (resourceState->policy == Policy::ArchiveBacked)
        return {};

    return stage(archiveEntry);
}

QString ProjectResourceStagingService::stage(const QString& archiveEntry, const CancellationCheck& cancellationCheck, const ProgressCallback& progress)
{
    const auto safeEntry = validateArchiveEntry(archiveEntry);
    QString archivePath;
    QString stagingDirectory;
    std::shared_ptr<ResourceState> resourceState;
    {
        std::lock_guard lock(_mutex);
        if (!_resources.contains(safeEntry))
            throw std::runtime_error(QString("Project resource was not registered for staging: %1").arg(safeEntry).toStdString());
        archivePath = _archivePath;
        stagingDirectory = _stagingDirectory;
        resourceState = _resources.value(safeEntry);
    }

    if (archivePath.isEmpty() || stagingDirectory.isEmpty())
        throw std::runtime_error("Project resource staging service is not configured");

    const auto destinationPath = QDir(stagingDirectory).filePath(safeEntry);
    const auto destinationDirectory = QFileInfo(destinationPath).absolutePath();
    if (!QDir().mkpath(destinationDirectory))
        throw std::runtime_error(QString("Unable to create staging directory for project resource: %1").arg(safeEntry).toStdString());

    const auto canonicalRoot = QFileInfo(stagingDirectory).canonicalFilePath();
    const auto canonicalDestinationDirectory = QFileInfo(destinationDirectory).canonicalFilePath();
    const auto rootWithSeparator = QDir::cleanPath(canonicalRoot) + QDir::separator();
    if (canonicalRoot.isEmpty() || canonicalDestinationDirectory.isEmpty() ||
        (canonicalDestinationDirectory != canonicalRoot && !canonicalDestinationDirectory.startsWith(rootWithSeparator, Qt::CaseInsensitive)))
        throw std::runtime_error(QString("Project resource destination escapes the staging directory: %1").arg(safeEntry).toStdString());

    {
        std::unique_lock lock(resourceState->mutex);
        while (resourceState->staging)
        {
            if (cancellationCheck && cancellationCheck())
                throw std::runtime_error(QString("Staging canceled for project resource: %1").arg(safeEntry).toStdString());
            resourceState->changed.wait_for(lock, std::chrono::milliseconds(50));
        }
        if (resourceState->ready && QFileInfo(destinationPath).isFile())
            return destinationPath;
        resourceState->staging = true;
    }

    const auto temporaryPath = destinationPath + QStringLiteral(".partial-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
    try
    {
        if (cancellationCheck && cancellationCheck())
            throw std::runtime_error(QString("Staging canceled for project resource: %1").arg(safeEntry).toStdString());

        util::Archiver archiver;
        archiver.extractSingleFileCancellable(archivePath, safeEntry, temporaryPath, {}, cancellationCheck, progress);
        if (cancellationCheck && cancellationCheck())
            throw std::runtime_error(QString("Staging canceled for project resource: %1").arg(safeEntry).toStdString());
        if (QFileInfo::exists(destinationPath))
            QFile::remove(destinationPath);
        if (!QFile::rename(temporaryPath, destinationPath))
            throw std::runtime_error(QString("Unable to publish staged project resource: %1").arg(safeEntry).toStdString());

        {
            std::lock_guard lock(resourceState->mutex);
            resourceState->ready = true;
            resourceState->staging = false;
        }
        resourceState->changed.notify_all();
        return destinationPath;
    }
    catch (const std::exception& exception)
    {
        QFile::remove(temporaryPath);
        {
            std::lock_guard lock(resourceState->mutex);
            resourceState->staging = false;
        }
        resourceState->changed.notify_all();
        throw std::runtime_error(QString("Unable to stage project resource %1: %2").arg(safeEntry, QString::fromUtf8(exception.what())).toStdString());
    }
    catch (...)
    {
        QFile::remove(temporaryPath);
        {
            std::lock_guard lock(resourceState->mutex);
            resourceState->staging = false;
        }
        resourceState->changed.notify_all();
        throw std::runtime_error(QString("Unable to stage project resource %1: unknown error").arg(safeEntry).toStdString());
    }
}

}
