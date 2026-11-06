// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <workflow/WorkflowPlan.h>

#include <QHash>
#include <QString>
#include <QStringList>

#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>

namespace mv
{

/** Session-scoped staging for resources stored inside a project archive. */
class ProjectResourceStagingService
{
public:

    enum class Policy
    {
        ArchiveBacked,
        OnDemand,
        BeforeLoad
    };

    using CancellationCheck = std::function<bool()>;
    using ProgressCallback = std::function<void(std::uint64_t, std::uint64_t)>;

    void configure(QString archivePath, QString stagingDirectory, Policy policy);
    void clear();
    void setPolicy(Policy policy);
    [[nodiscard]] Policy getPolicy() const;
    void registerResources(const QStringList& archiveEntries);
    [[nodiscard]] workflow::UniqueWorkflowPlan createEagerStagingWorkflow(std::size_t maxConcurrency);
    [[nodiscard]] QString resolvePath(const QString& archiveEntry);

private:

    struct ResourceState
    {
        std::mutex mutex;
        std::condition_variable changed;
        bool staging = false;
        bool ready = false;
    };

    [[nodiscard]] static QString validateArchiveEntry(const QString& archiveEntry);
    [[nodiscard]] QString stage(const QString& archiveEntry, const CancellationCheck& cancellationCheck = {}, const ProgressCallback& progress = {});

private:

    mutable std::mutex _mutex;                                      /**< Protects session configuration and registered requests. */
    QString _archivePath;                                           /**< Archive that owns the resources in this session. */
    QString _stagingDirectory;                                      /**< Session-only root for staged resources. */
    Policy _policy = Policy::ArchiveBacked;                         /**< Effective policy after applying the project override. */
    QHash<QString, std::shared_ptr<ResourceState>> _resources;      /**< Registered archive entries and their single-flight state. */
};

}
