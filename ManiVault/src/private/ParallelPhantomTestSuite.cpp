// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ParallelPhantomTestSuite.h"

#include <Application.h>
#include <BackgroundTask.h>
#include <CoreInterface.h>
#include <ModalTask.h>

#include <parallel/Parallel.h>

#include <util/StyledIcon.h>

#include <workflow/WorkflowOptions.h>
#include <workflow/WorkflowPlan.h>
#include <workflow/WorkflowResultFuture.h>

#include <QCoreApplication>
#include <QMetaObject>
#include <QPointer>
#include <QThread>
#include <QTimer>
#include <QVariantMap>
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

ModalTask* createModalTask(const QString& name, bool mayKill)
{
    ModalTask* task = nullptr;

    QMetaObject::invokeMethod(qApp, [&task, name, mayKill] {
        task = new ModalTask(nullptr, name, Task::Status::Undefined, mayKill);
        task->setMayKill(mayKill);
        task->setRunning();
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

void setTaskProgressDescription(const SharedWorkflowExecutionContext& context, const QString& description)
{
    if (auto task = context->getTask())
        task->setProgressDescription(description);
}

void runWorkLoop(const SharedWorkflowExecutionContext& context, std::int32_t steps, std::int32_t stepDelayMs, const QStringList& descriptions, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    for (std::int32_t step = 0; step < steps; ++step) {
        if (canceled && canceled->load())
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

            auto canceled = std::make_shared<std::atomic_bool>(false);
            auto task     = createModalTask("Parallel phantom modal cancellation", true);

            QObject::connect(task, &Task::requestAbort, task, [task, canceled] {
                canceled->store(true);
                task->setProgressDescription("Modal cancellation requested");
            });

            executeWithTask(makeParallelProgressPlan("Parallel phantom modal cancellation", 6, 120, 180, canceled), task, notificationOptions(), canceled);
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
        makeBackgroundProgressScenario(),
        makeModalProgressScenario(),
        makeNestedWorkflowScenario(),
        makeModalCancellationScenario(),
        makeCancellationScenario()
    };
}

void ParallelPhantomTestSuite::run(QObject* owner)
{
    static std::atomic_bool running = false;
    constexpr auto scenarioDelay = std::chrono::seconds(5);

    bool expected = false;

    if (!running.compare_exchange_strong(expected, true)) {
        postNotification("Parallel phantom suite already running", "Wait for the current phantom suite run to finish before starting another one.");
        return;
    }

    QPointer<QObject> safeOwner(owner);

    std::thread([safeOwner, scenarioDelay] {
        const auto scenarios = makeScenarios();

        postNotification("Starting parallel phantom suite", QStringLiteral("Running %1 hidden parallel workflow scenarios with a 5-second pause between scenarios.").arg(scenarios.size()));

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

            if (scenarioIndex + 1 < scenarios.size())
                std::this_thread::sleep_for(scenarioDelay);
        }

        postNotification("Parallel phantom suite finished", "All hidden parallel workflow scenarios have been started or completed.");

        running.store(false);
    }).detach();
}

}
