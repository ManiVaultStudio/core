// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowContextBase.h"
#include "WorkflowResult.h"
#include "WorkflowResultFuture.h"
#include "WorkflowExecutionOptions.h"

#include <QString>
#include <QVariant>

namespace mv
{
    class Task;
}

namespace mv::util
{

class AbstractWorkflowPlanExecutor;

using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;

class CORE_EXPORT WorkflowPlan
{
public:

    enum class ConcurrencyMode
    {
        Sequential,
        Parallel
    };

    enum class StageExecutionPolicy {
        Main,
        OnSuccess,
        OnFailure,
        Finally
    };

    enum class JobThreadAffinity {
        CurrentWorkerThread,
        GuiThread
    };

    enum class JobCompletionPolicy {
        Auto,
        ManualOrNested
    };

    enum class JobProgressMode {
        Automatic,  /** Default: leaf job auto-completes if no children */
        Atomic,     /** Ignore nested workflow progress; job is one unit */
        Nested      /** Nested workflows contribute fine-grained progress */
    };

    using SharedState = std::shared_ptr<QVariantMap>;

public:
    class Job;

    using JobFunction   = std::function<void(Job&)>;
    using JobFunctions  = std::vector<JobFunction>;

    class CORE_EXPORT Job
    {
    public:
        using ErrorString = QString;

        Job(QString name, JobFunction function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, JobProgressMode progressMode = JobProgressMode::Automatic);
        Job(QString name, JobFunction function, double weight);

        QString getName() const;

        const JobFunction& getFunction() const;

        void run() const;

        void setResult(QVariant result);

        const QVariant& getResult() const;

        void clearResult();

        void setError(QString error);

        bool           hasError() const;

        const QString& getError() const;

        void           clearError();

        void setException(const std::exception& e);

        void setUnknownException();

        void fail(QString error);

        JobThreadAffinity getThreadAffinity() const;

        double getWeight() const;

        Job weighted(double weight);

        JobProgressMode getProgressMode() const;

    private:
        QString     _name;
        JobFunction _function;
        QVariant    _result;
        std::optional<QString> _error;
        JobThreadAffinity _threadAffinity = JobThreadAffinity::CurrentWorkerThread;
        double          _weight = 1.0;
        JobProgressMode _progressMode = JobProgressMode::Automatic;
    };

    using Jobs = std::vector<Job>;

    class CORE_EXPORT Stage
    {
    public:

        Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs, double weight = 1.0);

        ConcurrencyMode getConcurrencyMode() const;

        QString getName() const;

        Jobs    getJobs() const;

        void setWeight(double weight);

        double getWeight() const;

        bool containsGuiThreadJobs() const;

        bool containsWorkerThreadJobs() const;

        bool isThreadAffinityCompatible() const;

    private:
        QString         _name;
        ConcurrencyMode _concurrencyMode;
        Jobs            _jobs;
        double          _weight = 1.0;
    };

    using Stages = std::vector<Stage>;

    WorkflowPlan(const QString& title, SharedWorkflowContext context = SharedWorkflowContext());

    void addStage(Stage stage);

    template<typename Function>
    void addSequentialStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 1.0) {
        addStageTo(_stages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    QString getName() const;

    void addSequentialStage(QString name, Jobs jobs, double weight = 1.0);
	void addParallelStage(QString name, Jobs jobs, double weight = 1.0);
    void addStage(QString name, ConcurrencyMode mode, Jobs jobs, double weight = 1.0);

    template<typename Function>
    void addOnSuccessStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_onSuccessStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    template<typename Function>
    void addOnFailureStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_onFailureStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    template<typename Function>
    void addFinalizationStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_finalizationStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    SharedWorkflowResult execute(const SharedWorkflowPlanExecutor& workflowPlanExecutor, WorkflowExecutionOptions executionOptions = {});
    WorkflowResultFuture executeAsync(const SharedWorkflowPlanExecutor& workflowPlanExecutor, WorkflowExecutionOptions executionOptions = {});
    SharedWorkflowResult executeOnCurrentThread(const SharedWorkflowPlanExecutor& workflowPlanExecutor, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {});

	Stages getStages() const;

    Stages getOnSuccessStages() const;
    Stages getOnFailureStages() const;
    Stages getFinallyStages() const;

    SharedState getSharedState() const;

    SharedWorkflowContext getWorkflowContext() const;

    template<typename WorkflowContextType>
    std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const {
        static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");
        return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
    }

    void setWeight(double weight);

    double getWeight() const;

private:
    template<typename Function>
    void addStageTo(Stages& stages, QString name, Function&& function, JobThreadAffinity threadAffinity, double weight) {
        if constexpr (std::is_invocable_v<Function, Job&>) {
            stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, {
                Job(name, JobFunction(std::forward<Function>(function)), threadAffinity)
                }, weight));
        }
        else if constexpr (std::is_invocable_v<Function>) {
            stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, {
                Job(name, JobFunction([fn = std::forward<Function>(function)](Job&) mutable {
                    fn();
                }), threadAffinity)
                }, weight));
        }
        else {
            static_assert(
                std::is_invocable_v<Function, Job&> || std::is_invocable_v<Function>,
                "Stage function must be callable as void(Job&) or void()"
                );
        }
    }

private:
    QString _name;
    Stages  _stages;
    Stages _onSuccessStages;
    Stages _onFailureStages;
    Stages _finalizationStages;
    SharedState _sharedState = std::make_shared<QVariantMap>();
    SharedWorkflowContext   _workflowContext;
    double _weight = 1.0;
    
};

}
