// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2026 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include <workflow/WorkflowPlan.h>
#include <util/ProjectResourceStagingPolicy.h>

#include <QHash>
#include <QList>
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

/**
 * @brief Resolves and stages project-archive resources for one open session.
 *
 * The service tracks per-resource policy and coordinates concurrent staging
 * so a resource is copied at most once at a time. Staged files live under the
 * session staging directory and their paths remain valid for that session.
 * Archive range reads remain available when staging is not selected. The
 * project manager owns the session lifecycle and uses eager staging workflows
 * to honor StageImmediately before data loading.
 *
 * @author Thomas Kroes
 */
class ProjectResourceStagingService
{
public:

    using Policy = util::ProjectResourceStagingPolicy;

    /** One archive entry and an optional policy override for that resource. */
    struct ResourceRequest
    {
        QString archiveEntry;            /**< Safe project-relative path of the resource in the archive. */
        Policy policy = Policy::Inherit; /**< Per-resource policy; Inherit uses the project and system defaults. */
    };

    /** Returns true when the owning workflow has requested cancellation. */
    using CancellationCheck = std::function<bool()>;
    /** Reports bytes copied and the total uncompressed resource size. */
    using ProgressCallback = std::function<void(std::uint64_t, std::uint64_t)>;

    /** Configures the archive, session directory, and system fallback policy for a project session. */
    void configure(QString archivePath, QString stagingDirectory, Policy policy);

    /** Clears session paths, policy overrides, and registered resource state. */
    void clear();

    /** Sets the policy saved on the project; Inherit preserves the system fallback. */
    void setProjectPolicy(Policy policy);

    /**
     * @brief Resolves a dataset policy against this session's project and system policies.
     * @param requestedPolicy Dataset-level policy, or Inherit to use the configured defaults
     * @return Effective policy, falling back to on-demand reads when every level inherits
     */
    [[nodiscard]] Policy resolvePolicy(Policy requestedPolicy) const;

    /** Resolves project and system settings to the policy used for inherited resources. */
    [[nodiscard]] Policy getEffectiveDefaultPolicy() const;

    /** Registers entries using the effective project policy. */
    void registerResources(const QStringList& archiveEntries);

    /** Registers entries with optional resource-specific policy overrides. */
    void registerResources(const QList<ResourceRequest>& requests);

    /** Creates a workflow that stages all registered StageImmediately resources. */
    [[nodiscard]] workflow::UniqueWorkflowPlan createEagerStagingWorkflow(std::size_t maxConcurrency);

    /** Returns an existing staged path or stages the entry if its policy requires it. */
    [[nodiscard]] QString resolvePath(const QString& archiveEntry);

private:

    struct ResourceState
    {
        std::mutex mutex;                           /**< Guards this resource's staging and readiness flags. */
        std::condition_variable changed;            /**< Wakes concurrent callers when staging finishes. */
        bool staging = false;                       /**< Whether one caller currently owns the staging operation. */
        bool ready = false;                         /**< Whether the complete resource is present in session storage. */
        Policy policy = Policy::ReadRangesOnDemand; /**< Resolved policy assigned when the request is registered. */
    };

    [[nodiscard]] static QString validateArchiveEntry(const QString& archiveEntry);
    [[nodiscard]] QString stage(const QString& archiveEntry, const CancellationCheck& cancellationCheck = {}, const ProgressCallback& progress = {});

private:

    mutable std::mutex _mutex;                                      /**< Protects session configuration and registered requests. */
    QString _archivePath;                                           /**< Archive that owns the resources in this session. */
    QString _stagingDirectory;                                      /**< Session-only root for staged resources. */
    Policy _systemPolicy = Policy::ReadRangesOnDemand;               /**< Application-wide fallback policy. */
    Policy _projectPolicy = Policy::Inherit;                        /**< Optional policy override for this project. */
    QHash<QString, std::shared_ptr<ResourceState>> _resources;      /**< Registered archive entries and their single-flight state. */
};

}
