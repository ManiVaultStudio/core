// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "ParallelPhantomTestSuite.h"

#include <Application.h>
#include <BackgroundTask.h>
#include <CoreInterface.h>

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

#include <atomic>
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

void runWorkLoop(const SharedWorkflowExecutionContext& context, std::int32_t steps, std::int32_t stepDelayMs, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    for (std::int32_t step = 0; step < steps; ++step) {
        if (canceled && canceled->load())
            throw std::runtime_error("Parallel phantom scenario was canceled");

        QThread::msleep(stepDelayMs);
        context->setProgress(static_cast<double>(step + 1) / static_cast<double>(steps));
    }
}

UniqueWorkflowPlan makeParallelProgressPlan(const QString& name, std::int32_t jobCount, std::int32_t steps, std::int32_t stepDelayMs, const std::shared_ptr<std::atomic_bool>& canceled = {})
{
    auto plan = std::make_unique<WorkflowPlan>(name);

    WorkflowPlan::Jobs jobs;

    jobs.reserve(jobCount);

    for (std::int32_t index = 0; index < jobCount; ++index) {
        jobs.emplace_back(QStringLiteral("Process phantom item %1").arg(index + 1), [index, steps, stepDelayMs, canceled](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
            context->info(QStringLiteral("Started phantom item %1").arg(index + 1), "ParallelPhantomTestSuite");
            runWorkLoop(context, steps, stepDelayMs, canceled);
        });
    }

    plan->addParallelStage("Process phantom items", std::move(jobs));
    plan->addSequentialStage("Summarize phantom work", [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& context) {
        context->warning("Synthetic warning from the phantom suite", "ParallelPhantomTestSuite");
        context->setProgress(1.0);
    });

    return plan;
}

void executeWithBackgroundTask(UniqueWorkflowPlan plan, BackgroundTask* task, WorkflowOptions options, const std::shared_ptr<std::atomic_bool>& canceled = {})
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
        }
    };
}

Scenario makeBackgroundProgressScenario()
{
    const QVariantMap parameters{
        { "jobs", 12 },
        { "steps", 20 },
        { "stepDelayMs", 75 },
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
            executeWithBackgroundTask(makeParallelProgressPlan("Parallel phantom background progress", 12, 20, 75), task, notificationOptions());
        }
    };
}

Scenario makeModalProgressScenario()
{
    const QVariantMap parameters{
        { "jobs", 5 },
        { "steps", 18 },
        { "stepDelayMs", 65 },
        { "taskScope", "Modal" },
        { "workflowOption", "reporting.progress" }
    };

    return {
        "Parallel modal progress",
        "Runs an asynchronous workflow through the standard progress-reporting option.",
        parameters,
        [parameters] {
            postNotification("Starting parallel phantom scenario", "Running the modal-progress scenario.", parameters);
            runAsyncModalProgressScenario(makeParallelProgressPlan("Parallel phantom modal progress", 5, 18, 65), progressNotificationOptions());
        }
    };
}

Scenario makeCancellationScenario()
{
    const QVariantMap parameters{
        { "jobs", 8 },
        { "steps", 40 },
        { "stepDelayMs", 80 },
        { "autoCancelAfterMs", 900 },
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
                QTimer::singleShot(900, task, [task] {
                    if (task)
                        task->kill();
                });
            }, Qt::QueuedConnection);

            postTaskNotification(task);
            executeWithBackgroundTask(makeParallelProgressPlan("Parallel phantom cancellation", 8, 40, 80, canceled), task, notificationOptions(), canceled);
        }
    };
}

std::vector<Scenario> makeScenarios()
{
    return {
        makeSmokeScenario(),
        makeBackgroundProgressScenario(),
        makeModalProgressScenario(),
        makeCancellationScenario()
    };
}

void ParallelPhantomTestSuite::run(QObject* owner)
{
    static std::atomic_bool running = false;

    bool expected = false;

    if (!running.compare_exchange_strong(expected, true)) {
        postNotification("Parallel phantom suite already running", "Wait for the current phantom suite run to finish before starting another one.");
        return;
    }

    QPointer<QObject> safeOwner(owner);

    std::thread([safeOwner] {
        const auto scenarios = makeScenarios();

        postNotification("Starting parallel phantom suite", QStringLiteral("Running %1 hidden parallel workflow scenarios.").arg(scenarios.size()));

        for (const auto& scenario : scenarios) {
            if (!safeOwner)
                break;

            try {
                scenario.run();
            }
            catch (const std::exception& exception) {
                postNotification(QStringLiteral("%1 failed").arg(scenario.name), QString::fromUtf8(exception.what()));
            }
            catch (...) {
                postNotification(QStringLiteral("%1 failed").arg(scenario.name), "Unknown exception.");
            }
        }

        postNotification("Parallel phantom suite finished", "All hidden parallel workflow scenarios have been started or completed.");

        running.store(false);
    }).detach();
}

}
