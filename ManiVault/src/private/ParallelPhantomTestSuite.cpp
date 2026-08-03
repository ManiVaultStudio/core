// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ParallelPhantomTestSuite.h"

#include <Application.h>
#include <BackgroundTask.h>
#include <CoreInterface.h>

#include <parallel/Parallel.h>

#include <util/StyledIcon.h>
#include <util/SeverityLevel.h>

#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowResultFuture.h>

#include <QCoreApplication>
#include <QAction>
#include <QCursor>
#include <QMenu>
#include <QMetaObject>
#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QVariantMap>
#include <QWidget>
#include <QStringList>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

using namespace mv;
using namespace mv::workflow;

namespace mv::detail
{

struct Scenario
{
    QString name;
    QString description;
    QVariantMap parameters;
    std::function<void()> run;
};

QString toParameterSummary(const QVariantMap& parameters)
{
    QStringList entries;

    for (auto iterator = parameters.constBegin(); iterator != parameters.constEnd(); ++iterator)
        entries << QStringLiteral("%1=%2").arg(iterator.key(), iterator.value().toString());

    return entries.join(", ");
}

void postNotification(QString title, QString description, QVariantMap parameters = {})
{
    const auto parameterSummary = toParameterSummary(parameters);

    if (!parameterSummary.isEmpty())
        description = QStringLiteral("%1<br/><br/><b>Parameters:</b> %2").arg(description, parameterSummary);

    QMetaObject::invokeMethod(&help(), [title = std::move(title), description = std::move(description)] {
        help().addNotification(title, description, util::StyledIcon("flask"));
    }, Qt::QueuedConnection);
}

void postTaskNotification(QPointer<Task> task)
{
    QMetaObject::invokeMethod(&help(), [task] {
        if (task)
            help().addNotification(task);
    }, Qt::QueuedConnection);
}

void cleanupTaskLater(QPointer<Task> task)
{
    QMetaObject::invokeMethod(qApp, [task] {
        if (task)
            QTimer::singleShot(30000, task, &QObject::deleteLater);
    }, Qt::QueuedConnection);
}

BackgroundTask* createBackgroundTask(const QString& name, bool mayKill)
{
    BackgroundTask* task = nullptr;

    QMetaObject::invokeMethod(qApp, [&task, name, mayKill] {
        task = new BackgroundTask(nullptr, name, true, Task::Status::Undefined, mayKill);
        task->setMayKill(mayKill);
        task->setRunning();
    }, Qt::BlockingQueuedConnection);

    return task;
}

Task* createModalTask(const QString& name, Task::Status status, bool mayKill)
{
    Task* task = nullptr;

    QMetaObject::invokeMethod(qApp, [&task, name, status, mayKill] {
        task = new Task(nullptr, name, { Task::GuiScope::Modal }, status, mayKill);
        task->setMayKill(mayKill);
    }, Qt::BlockingQueuedConnection);

    return task;
}

WorkflowOptions notificationOptions()
{
    WorkflowOptions options;

    options.reporting.finishedNotification = true;

    return options;
}

WorkflowOptions progressNotificationOptions()
{
    auto options = notificationOptions();

    options.reporting.progress = true;

    return options;
}

WorkflowOptions progressCancellationNotificationOptions()
{
    auto options = progressNotificationOptions();

    options.cancellation.enabled = true;

    return options;
}

void setTaskProgressDescription(const SharedWorkflowExecutionContext& context, const QString& description)
{
    if (auto task = context->getTask())
        task->setProgressDescription(description);
}

bool isCancellationRequested(const SharedWorkflowExecutionContext& context)
{
    const auto task = context->getTask();

    return task && (task->isAboutToBeAborted() || task->isAborting() || task->isAborted());
}

void runWorkLoop(const SharedWorkflowExecutionContext& context, std::int32_t steps, std::int32_t stepDelayMs, const QStringList& descriptions, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    for (std::int32_t step = 0; step < steps; ++step) {
        if ((canceled && canceled->load()) || isCancellationRequested(context))
            throw std::runtime_error("Parallel phantom scenario was canceled");

        if (!descriptions.isEmpty())
            setTaskProgressDescription(context, descriptions[step % descriptions.size()]);

        QThread::msleep(stepDelayMs);
        context->setProgress(static_cast<double>(step + 1) / static_cast<double>(steps));
    }
}

UniqueWorkflowPlan makeParallelProgressPlan(const QString& name, std::int32_t jobCount, std::int32_t steps, std::int32_t stepDelayMs, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    auto plan = std::make_unique<WorkflowPlan>(name);
    const QStringList jobNames{
        "Opening dataset header",
        "Reading point coordinates",
        "Reading dimension names",
        "Decoding selection groups",
        "Loading color maps",
        "Resolving view presets",
        "Saving preview cache",
        "Writing project manifest",
        "Compressing workspace state",
        "Updating recent projects",
        "Indexing source metadata",
        "Closing archive stream"
    };

    plan->addSequentialStage("Open phantom project file", [steps, stepDelayMs, canceled](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Opening project file",
            "Reading archive header",
            "Validating project version",
            "Preparing workflow context"
        }, canceled);
    });

    WorkflowPlan::Jobs jobs;

    jobs.reserve(jobCount);

    for (std::int32_t index = 0; index < jobCount; ++index) {
        const auto jobName = jobNames[index % jobNames.size()];

        jobs.emplace_back(jobName, [jobName, steps, stepDelayMs, canceled](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            context->info(QStringLiteral("Started %1").arg(jobName), "ParallelPhantomTestSuite");
            runWorkLoop(context, steps, stepDelayMs, {
                QStringLiteral("%1: scanning blocks").arg(jobName),
                QStringLiteral("%1: transforming records").arg(jobName),
                QStringLiteral("%1: updating progress").arg(jobName),
                QStringLiteral("%1: committing results").arg(jobName)
            }, canceled);
        });
    }

    plan->addParallelStage("Read and transform phantom data", std::move(jobs));
    plan->addSequentialStage("Save phantom project file", [steps, stepDelayMs, canceled](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Saving project manifest",
            "Writing workspace layout",
            "Flushing compressed blocks",
            "Updating recent-project entry"
        }, canceled);
    });
    plan->addSequentialStage("Summarize phantom work", [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        setTaskProgressDescription(context, "Writing workflow summary");
        context->warning("Synthetic warning from the phantom suite", "ParallelPhantomTestSuite");
        context->setProgress(1.0);
    });

    return plan;
}

UniqueWorkflowPlan makeViewRestoreWorkflow(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Open view preset", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Opening view preset",
            "Reading camera state",
            "Reading layer visibility",
            "Preparing renderer settings"
        });
    });

    WorkflowPlan::Jobs jobs;

    jobs.emplace_back("Restore scatterplot view", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Restoring scatterplot view",
            "Binding coordinate dimensions",
            "Applying color mapping",
            "Refreshing visible selection"
        });
    });

    jobs.emplace_back("Restore table view", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Restoring table view",
            "Rebuilding visible columns",
            "Restoring sort order",
            "Applying row filters"
        });
    });

    jobs.emplace_back("Restore differential expression view", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Restoring differential expression view",
            "Loading comparison groups",
            "Recomputing summary labels",
            "Refreshing volcano plot"
        });
    });

    plan->addParallelStage("Restore view plugins", std::move(jobs));
    plan->addSequentialStage("Activate restored views", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Activating restored views",
            "Docking view panels",
            "Publishing current view state"
        });
    });

    return plan;
}

UniqueWorkflowPlan makeDatasetRestoreWorkflow(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Read dataset metadata", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Reading dataset metadata",
            "Resolving dimension aliases",
            "Validating sparse matrix blocks",
            "Preparing derived statistics"
        });
    });

    plan->addNestedWorkflowStage("Restore nested view workflow", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return makeViewRestoreWorkflow("Restore linked views", steps, stepDelayMs);
    });

    plan->addSequentialStage("Publish dataset availability", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Publishing dataset availability",
            "Updating dataset picker options",
            "Refreshing data hierarchy"
        });
    });

    return plan;
}

UniqueWorkflowPlan makeNestedWorkflowPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Open project container", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Opening project container",
            "Reading root manifest",
            "Resolving storage codecs",
            "Preparing nested restore workflow"
        });
    });

    plan->addNestedWorkflowStage("Restore nested dataset workflow", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return makeDatasetRestoreWorkflow("Restore primary dataset", steps, stepDelayMs);
    });

    plan->addSequentialStage("Finalize nested project restore", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Finalizing nested project restore",
            "Saving recovered layout",
            "Writing restore summary"
        });
    });

    return plan;
}

UniqueWorkflowPlan makeNestedFailurePlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Open project container", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps / 2, stepDelayMs, {
            "Opening project container",
            "Reading root manifest",
            "Preparing nested failure test"
        });
    });

    plan->addNestedWorkflowStage("Restore nested workflow with failing job", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        auto nestedPlan = std::make_unique<WorkflowPlan>("Restore dataset with failing view");

        nestedPlan->addSequentialStage("Read dataset metadata", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            runWorkLoop(context, steps / 2, stepDelayMs, {
                "Reading dataset metadata",
                "Resolving dimension aliases",
                "Preparing failing view restore"
            });
        });

        nestedPlan->addSequentialStage("Restore failing view plugin", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            runWorkLoop(context, steps / 2, stepDelayMs, {
                "Restoring view plugin",
                "Binding dimensions",
                "Injecting synthetic nested failure"
            });

            throw std::runtime_error("Synthetic nested workflow failure");
        });

        nestedPlan->addOnFailureStage("Collect nested failure diagnostics", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            setTaskProgressDescription(context, "Collecting nested failure diagnostics");
            QThread::msleep(static_cast<unsigned long>(steps * stepDelayMs / 2));
            context->warning("Nested failure diagnostics were collected", "ParallelPhantomTestSuite");
        });

        nestedPlan->addFinalizationStage("Release nested failure resources", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            setTaskProgressDescription(context, "Releasing nested failure resources");
            QThread::msleep(static_cast<unsigned long>(steps * stepDelayMs / 3));
        });

        return nestedPlan;
    });

    plan->addOnFailureStage("Collect root failure diagnostics", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        setTaskProgressDescription(context, "Collecting root failure diagnostics");
        QThread::msleep(static_cast<unsigned long>(steps * stepDelayMs / 2));
        context->warning("Root failure diagnostics were collected", "ParallelPhantomTestSuite");
    });

    plan->addFinalizationStage("Release root failure resources", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        setTaskProgressDescription(context, "Releasing root failure resources");
        QThread::msleep(static_cast<unsigned long>(steps * stepDelayMs / 3));
    });

    return plan;
}

UniqueWorkflowPlan makeReportingMatrixPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Collect reporting samples", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Collecting informational messages",
            "Collecting warning messages",
            "Collecting error messages",
            "Collecting fatal messages"
        });

        context->info("Synthetic informational workflow message", "ParallelPhantomTestSuite");
        context->warning("Synthetic warning workflow message", "ParallelPhantomTestSuite");
        context->error("Synthetic error workflow message", "ParallelPhantomTestSuite");
        context->message(util::SeverityLevel::Fatal, "Synthetic fatal workflow message", "ParallelPhantomTestSuite", {});
    });

    return plan;
}

UniqueWorkflowPlan makeIndeterminateProgressPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Stream archive without size estimate", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        const QStringList descriptions{
            "Opening stream",
            "Reading next compressed block",
            "Decoding records without total size",
            "Publishing streamed records"
        };

        for (std::int32_t step = 0; step < steps; ++step) {
            if (isCancellationRequested(context))
                throw std::runtime_error("Parallel phantom scenario was canceled");

            setTaskProgressDescription(context, descriptions[step % descriptions.size()]);
            QThread::msleep(stepDelayMs);
        }
    });

    return plan;
}

UniqueWorkflowPlan makeManyTinyJobsPlan(const QString& name, std::int32_t jobCount, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);
    WorkflowPlan::Jobs jobs;

    jobs.reserve(jobCount);

    for (std::int32_t index = 0; index < jobCount; ++index) {
        const auto jobName = QStringLiteral("Decode tiny block %1").arg(index + 1);

        jobs.emplace_back(jobName, [jobName, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            setTaskProgressDescription(context, jobName);
            QThread::msleep(stepDelayMs);
            context->setProgress(1.0);
        });
    }

    plan->addParallelStage("Decode many tiny blocks", std::move(jobs));

    return plan;
}

UniqueWorkflowPlan makeGuiThreadPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Prepare worker-side state", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Preparing worker-side state",
            "Packing UI update payload",
            "Scheduling GUI-thread stage"
        });
    });

    plan->addSequentialStage("Apply GUI-thread update", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        setTaskProgressDescription(context, "Applying GUI-thread update");

        if (QThread::currentThread() != qApp->thread())
            context->error("GUI-thread stage did not run on the GUI thread", "ParallelPhantomTestSuite");
        else
            context->info("GUI-thread stage ran on the GUI thread", "ParallelPhantomTestSuite");

        QThread::msleep(static_cast<unsigned long>(steps * stepDelayMs));
        context->setProgress(1.0);
    }, WorkflowPlan::JobThreadAffinity::GuiThread);

    plan->addSequentialStage("Resume worker-side state", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Resuming worker-side state",
            "Verifying GUI-thread result",
            "Completing mixed-affinity workflow"
        });
    });

    return plan;
}

UniqueWorkflowPlan makeLongSingleJobPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Import one large file", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Opening large file",
            "Reading large file",
            "Parsing large file",
            "Saving large file import"
        });
    });

    return plan;
}

UniqueWorkflowPlan makeNoProgressPlan(const QString& name, std::int32_t steps, std::int32_t stepDelayMs)
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    plan->addSequentialStage("Run headless workflow", [steps, stepDelayMs](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        runWorkLoop(context, steps, stepDelayMs, {
            "Running headless workflow",
            "Collecting headless output",
            "Completing without task progress UI"
        });
    });

    return plan;
}

void executeWithTask(UniqueWorkflowPlan plan, Task* task, WorkflowOptions options, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    QPointer<Task> safeTask(task);

    try {
        const auto result = Application::getWorkflowPlanExecutor().executeBlocking(std::move(plan), task, options);

        Q_UNUSED(result);
    }
    catch (...) {
        if (safeTask) {
            QMetaObject::invokeMethod(safeTask.data(), [safeTask] {
                if (safeTask)
                    safeTask->setAborted();
            }, Qt::QueuedConnection);
        }

        cleanupTaskLater(safeTask);
        throw;
    }

    if (safeTask && canceled && canceled->load()) {
        QMetaObject::invokeMethod(safeTask.data(), [safeTask] {
            if (safeTask)
                safeTask->setAborted();
        }, Qt::QueuedConnection);
    }

    cleanupTaskLater(safeTask);
}

void runAsyncModalProgressScenario(UniqueWorkflowPlan plan, WorkflowOptions options)
{
    WorkflowResultFuture future;
    auto planPtr = plan.release();

    QMetaObject::invokeMethod(qApp, [&future, planPtr, options] {
        future = Application::getWorkflowPlanExecutor().execute(UniqueWorkflowPlan(planPtr), nullptr, options);
    }, Qt::BlockingQueuedConnection);

    const auto result = future.get();

    Q_UNUSED(result);
}

Scenario makeSmokeScenario()
{
    const QVariantMap parameters{
        { "notifications", "finished workflow result" },
        { "mode", "blocking" }
    };

    return {
        "Parallel phantom smoke test",
        "Runs the existing phantom run, forEach, map, and staged-chain smoke test.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the smoke-test scenario.", parameters);
            Parallel::runPhantomTest(notificationOptions());
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    };
}

Scenario makeBackgroundProgressScenario()
{
    const QVariantMap parameters{
        { "jobs", 8 },
        { "steps", 50 },
        { "stepDelayMs", 150 },
        { "taskScope", "Background" },
        { "taskNotification", true }
    };

    return {
        "Parallel background progress",
        "Runs parallel work with a background task so progress is visible in the status bar and as a task notification.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the background-progress scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom background progress", false);

            postTaskNotification(task);
            executeWithTask(makeParallelProgressPlan("Parallel phantom background progress", 8, 50, 150), task, notificationOptions());
        }
    };
}

Scenario makeModalProgressScenario()
{
    const QVariantMap parameters{
        { "jobs", 6 },
        { "steps", 45 },
        { "stepDelayMs", 140 },
        { "taskScope", "Modal" },
        { "workflowOption", "reporting.progress" }
    };

    return {
        "Parallel modal progress",
        "Runs an asynchronous workflow through the standard progress-reporting option.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the modal-progress scenario.", parameters);
            runAsyncModalProgressScenario(makeParallelProgressPlan("Parallel phantom modal progress", 6, 45, 140), progressNotificationOptions());
        }
    };
}

Scenario makeNestedWorkflowScenario()
{
    const QVariantMap parameters{
        { "nestingDepth", 3 },
        { "mode", "nested WorkflowPlan" },
        { "steps", 36 },
        { "stepDelayMs", 120 },
        { "taskScope", "Background" },
        { "taskNotification", true }
    };

    return {
        "Parallel nested workflow progress",
        "Runs a three-level nested workflow with sequential and parallel stages at different depths.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the nested-workflow scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom nested workflow", false);

            postTaskNotification(task);
            executeWithTask(makeNestedWorkflowPlan("Parallel phantom nested workflow", 36, 120), task, notificationOptions());
        }
    };
}

Scenario makeNestedFailureScenario()
{
    const QVariantMap parameters{
        { "nestingDepth", 2 },
        { "failure", "synthetic nested exception" },
        { "steps", 30 },
        { "stepDelayMs", 90 },
        { "taskScope", "Background" },
        { "onFailureStages", true },
        { "finallyStages", true }
    };

    return {
        "Parallel nested failure reporting",
        "Runs a nested workflow that fails and then executes failure and finalization stages.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the nested-failure scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom nested failure", false);

            postTaskNotification(task);
            executeWithTask(makeNestedFailurePlan("Parallel phantom nested failure", 30, 90), task, notificationOptions());
        }
    };
}

Scenario makeNestedCancellationScenario()
{
    const QVariantMap parameters{
        { "nestingDepth", 3 },
        { "steps", 52 },
        { "stepDelayMs", 120 },
        { "autoCancelAfterMs", 3500 },
        { "taskScope", "Background" },
        { "workflowOption", "cancellation.enabled" }
    };

    return {
        "Parallel nested cancellation",
        "Runs a killable nested workflow and automatically requests cancellation while nested stages are active.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the nested-cancellation scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom nested cancellation", true);
            auto options = notificationOptions();

            options.cancellation.enabled = true;

            QObject::connect(task, &Task::requestAbort, task, [task] {
                task->setProgressDescription("Nested cancellation requested");
            });

            QMetaObject::invokeMethod(qApp, [task] {
                QTimer::singleShot(3500, task, [task] {
                    if (task)
                        task->kill();
                });
            }, Qt::QueuedConnection);

            postTaskNotification(task);
            executeWithTask(makeNestedWorkflowPlan("Parallel phantom nested cancellation", 52, 120), task, options);
        }
    };
}

Scenario makeModalCancellationScenario()
{
    const QVariantMap parameters{
        { "jobs", 6 },
        { "steps", 120 },
        { "stepDelayMs", 180 },
        { "cancelMode", "manual modal task dialog" },
        { "taskScope", "Modal" }
    };

    return {
        "Parallel modal cancellation",
        "Runs a killable modal task so cancellation can be requested from the modal task dialog.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the modal-cancellation scenario. Use the modal task dialog to cancel this task.", parameters);
            runAsyncModalProgressScenario(makeParallelProgressPlan("Parallel phantom modal cancellation", 6, 120, 180), progressCancellationNotificationOptions());
        }
    };
}

Scenario makeReportingMatrixScenario()
{
    const QVariantMap parameters{
        { "messages", "info, warning, error, fatal" },
        { "steps", 24 },
        { "stepDelayMs", 80 },
        { "taskScope", "Background" },
        { "finishedNotification", true }
    };

    return {
        "Parallel reporting matrix",
        "Runs a workflow that emits informational, warning, error, and fatal messages.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the reporting-matrix scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom reporting matrix", false);

            postTaskNotification(task);
            executeWithTask(makeReportingMatrixPlan("Parallel phantom reporting matrix", 24, 80), task, notificationOptions());
        }
    };
}

Scenario makeIndeterminateProgressScenario()
{
    const QVariantMap parameters{
        { "steps", 40 },
        { "stepDelayMs", 160 },
        { "progressMode", "RunningIndeterminate" },
        { "taskScope", "Background" }
    };

    return {
        "Parallel indeterminate progress",
        "Runs a background task that updates progress text without reporting a known progress fraction.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the indeterminate-progress scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom indeterminate progress", false);

            QMetaObject::invokeMethod(task, [task] {
                task->setRunningIndeterminate();
            }, Qt::BlockingQueuedConnection);

            postTaskNotification(task);
            executeWithTask(makeIndeterminateProgressPlan("Parallel phantom indeterminate progress", 40, 160), task, notificationOptions());
        }
    };
}

Scenario makeModalIndeterminateProgressScenario()
{
    const QVariantMap parameters{
        { "steps", 40 },
        { "stepDelayMs", 160 },
        { "progressMode", "RunningIndeterminate" },
        { "taskScope", "Modal" }
    };

    return {
        "Parallel modal indeterminate progress",
        "Runs a modal task that updates progress text without reporting a known progress fraction.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the modal-indeterminate-progress scenario.", parameters);

            auto task = createModalTask("Parallel phantom modal indeterminate progress", Task::Status::RunningIndeterminate, false);

            executeWithTask(makeIndeterminateProgressPlan("Parallel phantom modal indeterminate progress", 40, 160), task, notificationOptions());
        }
    };
}

Scenario makeManyTinyJobsScenario()
{
    const QVariantMap parameters{
        { "jobs", 180 },
        { "stepDelayMs", 18 },
        { "taskScope", "Background" },
        { "purpose", "aggregation overhead" }
    };

    return {
        "Parallel many tiny jobs",
        "Runs many short jobs to exercise scheduling, aggregation, and UI-update throttling.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the many-tiny-jobs scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom many tiny jobs", false);

            postTaskNotification(task);
            executeWithTask(makeManyTinyJobsPlan("Parallel phantom many tiny jobs", 180, 18), task, notificationOptions());
        }
    };
}

Scenario makeGuiThreadScenario()
{
    const QVariantMap parameters{
        { "steps", 18 },
        { "stepDelayMs", 80 },
        { "threadAffinity", "GuiThread" },
        { "taskScope", "Background" }
    };

    return {
        "Parallel GUI-thread affinity",
        "Runs a workflow with a GUI-thread stage between worker-thread stages.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the GUI-thread-affinity scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom GUI-thread affinity", false);

            postTaskNotification(task);
            executeWithTask(makeGuiThreadPlan("Parallel phantom GUI-thread affinity", 18, 80), task, notificationOptions());
        }
    };
}

Scenario makeLongSingleJobScenario()
{
    const QVariantMap parameters{
        { "jobs", 1 },
        { "steps", 110 },
        { "stepDelayMs", 100 },
        { "taskScope", "Background" }
    };

    return {
        "Parallel long single job",
        "Runs one long job to exercise progress text and graphical progress without parallel aggregation.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the long-single-job scenario.", parameters);

            auto task = createBackgroundTask("Parallel phantom long single job", false);

            postTaskNotification(task);
            executeWithTask(makeLongSingleJobPlan("Parallel phantom long single job", 110, 100), task, notificationOptions());
        }
    };
}

Scenario makeNoProgressReportingScenario()
{
    const QVariantMap parameters{
        { "steps", 24 },
        { "stepDelayMs", 90 },
        { "taskScope", "None" },
        { "finishedNotification", true }
    };

    return {
        "Parallel no progress reporting",
        "Runs a workflow with finished notifications but no task progress UI.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the no-progress-reporting scenario.", parameters);
            const auto result = Application::getWorkflowPlanExecutor().executeBlocking(makeNoProgressPlan("Parallel phantom no progress reporting", 24, 90), nullptr, notificationOptions());

            Q_UNUSED(result);
        }
    };
}

Scenario makeCancellationScenario()
{
    const QVariantMap parameters{
        { "jobs", 8 },
        { "steps", 80 },
        { "stepDelayMs", 100 },
        { "autoCancelAfterMs", 3000 },
        { "taskScope", "Background" }
    };

    return {
        "Parallel background cancellation",
        "Runs a killable background task and automatically requests cancellation while parallel jobs are running.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the cancellation scenario.", parameters);

            auto canceled = std::make_shared<std::atomic_bool>(false);
            auto task     = createBackgroundTask("Parallel phantom cancellation", true);

            QObject::connect(task, &Task::requestAbort, task, [task, canceled] {
                canceled->store(true);
                task->setProgressDescription("Cancellation requested");
            });

            QMetaObject::invokeMethod(qApp, [task] {
                QTimer::singleShot(3000, task, [task] {
                    if (task)
                        task->kill();
                });
            }, Qt::QueuedConnection);

            postTaskNotification(task);
            executeWithTask(makeParallelProgressPlan("Parallel phantom cancellation", 8, 80, 100, canceled), task, notificationOptions(), canceled);
        }
    };
}

std::vector<Scenario> makeScenarios()
{
    return {
        makeSmokeScenario(),
        makeNoProgressReportingScenario(),
        makeBackgroundProgressScenario(),
        makeModalProgressScenario(),
        makeNestedWorkflowScenario(),
        makeNestedFailureScenario(),
        makeNestedCancellationScenario(),
        makeModalCancellationScenario(),
        makeCancellationScenario(),
        makeReportingMatrixScenario(),
        makeIndeterminateProgressScenario(),
        makeModalIndeterminateProgressScenario(),
        makeManyTinyJobsScenario(),
        makeGuiThreadScenario(),
        makeLongSingleJobScenario()
    };
}

std::atomic_bool& running()
{
    static std::atomic_bool running = false;

    return running;
}

void runScenarios(QObject* owner, std::vector<Scenario> scenarios, bool pauseBetweenScenarios)
{
    constexpr auto scenarioDelay = std::chrono::seconds(5);

    bool expected = false;

    if (!running().compare_exchange_strong(expected, true)) {
        postNotification("Parallel phantom suite already running", "Wait for the current phantom suite run to finish before starting another one.");
        return;
    }

    QPointer<QObject> safeOwner(owner);

    std::thread([safeOwner, scenarios = std::move(scenarios), pauseBetweenScenarios, scenarioDelay] {
        postNotification("Starting parallel phantom suite", QStringLiteral("Running %1 hidden parallel workflow scenario%2%3.")
            .arg(scenarios.size())
            .arg(scenarios.size() == 1 ? "" : "s")
            .arg(pauseBetweenScenarios && scenarios.size() > 1 ? " with a 5-second pause between scenarios" : ""));

        for (auto scenarioIndex = std::size_t{ 0 }; scenarioIndex < scenarios.size(); ++scenarioIndex) {
            if (!safeOwner)
                break;

            const auto& scenario = scenarios[scenarioIndex];

            try {
                scenario.run();
            }
            catch (const std::exception& exception) {
                postNotification(QStringLiteral("%1 failed").arg(scenario.name), QString::fromUtf8(exception.what()));
            }
            catch (...) {
                postNotification(QStringLiteral("%1 failed").arg(scenario.name), "Unknown exception.");
            }

            if (pauseBetweenScenarios && scenarioIndex + 1 < scenarios.size())
                std::this_thread::sleep_for(scenarioDelay);
        }

        postNotification("Parallel phantom suite finished", "All hidden parallel workflow scenarios have been started or completed.");

        running().store(false);
    }).detach();
}

void runScenario(QObject* owner, std::size_t scenarioIndex)
{
    auto scenarios = makeScenarios();

    if (scenarioIndex >= scenarios.size()) {
        postNotification("Parallel phantom scenario unavailable", QStringLiteral("No hidden parallel workflow scenario exists at index %1.").arg(scenarioIndex));
        return;
    }

    std::vector<Scenario> selectedScenarios;

    selectedScenarios.emplace_back(std::move(scenarios[scenarioIndex]));
    runScenarios(owner, std::move(selectedScenarios), false);
}

void ParallelPhantomTestSuite::run(QObject* owner)
{
    runScenarios(owner, makeScenarios(), true);
}

void ParallelPhantomTestSuite::showMenu(QWidget* parent)
{
    auto scenarios = makeScenarios();
    QMenu menu(parent);

    menu.setTitle("Parallel Phantom Tests");

    auto runAllAction = menu.addAction("Run all scenarios sequentially");

    runAllAction->setData(-1);
    menu.addSeparator();

    auto singleScenarioMenu = menu.addMenu("Run single scenario");

    for (auto scenarioIndex = std::size_t{ 0 }; scenarioIndex < scenarios.size(); ++scenarioIndex) {
        auto action = singleScenarioMenu->addAction(scenarios[scenarioIndex].name);

        action->setData(static_cast<int>(scenarioIndex));
        action->setToolTip(scenarios[scenarioIndex].description);
    }

    const auto selectedAction = menu.exec(QCursor::pos());

    if (selectedAction == nullptr)
        return;

    const auto selectedIndex = selectedAction->data().toInt();

    if (selectedIndex < 0) {
        run(parent);
        return;
    }

    runScenario(parent, static_cast<std::size_t>(selectedIndex));
}

}
