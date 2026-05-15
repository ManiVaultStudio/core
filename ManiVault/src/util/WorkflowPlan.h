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

#include <optional>

namespace mv
{
    class Task;
}

namespace mv::util
{

class AbstractWorkflowPlanExecutor;

class WorkflowExecutionContext;

using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;
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

    public: // Workflow context access for jobs

        /**
         * @brief Gets the shared workflow context associated with this job.
         * @return A SharedWorkflowContext object that provides access to the shared workflow context for this job.
         */
        SharedWorkflowContext getWorkflowContext() const;

        /**
         * @brief Gets the shared workflow context associated with this job, cast to the specified type.
         * @tparam WorkflowContextType The specific derived type of WorkflowContextBase that you want to access the workflow context as.
         * @return A std::shared_ptr<WorkflowContextType> that points to the workflow context cast to the specified type if the cast is successful, or nullptr if the cast fails.
         */
        template<typename WorkflowContextType>
        std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const {
            static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");

            return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
        }

    protected: // Workflow context access for jobs

        /**
         * @brief Sets the shared workflow context for this job.
         * @param workflowContext The shared workflow context to be associated with this job.
         */
        void setWorkflowContext(SharedWorkflowContext workflowContext);

    private:
        QString                     _name;
        JobFunction                 _function;
        QVariant                    _result;
        std::optional<QString>      _error;
        JobThreadAffinity           _threadAffinity = JobThreadAffinity::CurrentWorkerThread;
        double                      _weight = 1.0;
        JobProgressMode             _progressMode = JobProgressMode::Automatic;
        SharedWorkflowContext       _workflowContext;

        friend class WorkflowPlan;
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

    SharedWorkflowResult executeBlocking(const SharedWorkflowPlanExecutor& workflowPlanExecutor, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = {});
    WorkflowResultFuture executeAsync(const SharedWorkflowPlanExecutor& workflowPlanExecutor, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = {});
    SharedWorkflowResult executeOnCurrentThread(const SharedWorkflowPlanExecutor& workflowPlanExecutor, Task* task, SharedWorkflowExecutionContext parentContext = nullptr, OptionalWorkflowExecutionOptions executionOptions = {});

	Stages getStages() const;

    Stages getOnSuccessStages() const;
    Stages getOnFailureStages() const;
    Stages getFinallyStages() const;

    /**
     * @brief Gets the shared workflow context associated with this workflow plan. The workflow context is a shared data structure that can be used to store and share data across stages and jobs within this workflow plan.
     * @return A SharedWorkflowContext object that provides access to the shared workflow context for this workflow plan.
     */
    SharedWorkflowContext getWorkflowContext() const;

    /**
     * @brief Gets the shared workflow context associated with this workflow plan, cast to the specified type. This is a convenience method that allows you to access the workflow context as a specific derived type without having to manually perform the dynamic pointer cast each time. The template parameter WorkflowContextType must be a type that derives from WorkflowContextBase, and the method will return a shared pointer to the workflow context cast to that type if the cast is successful, or nullptr if the cast fails (e.g., if the actual type of the workflow context is not compatible with WorkflowContextType).
     * @tparam WorkflowContextType The specific derived type of WorkflowContextBase that you want to access the workflow context as.
     * @return A std::shared_ptr<WorkflowContextType> that points to the workflow context cast to the specified type, or nullptr if the cast fails.
     */
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
    QString                 _name;                  /** Name of the workflow plan */
    Stages                  _stages;                /** Stages to execute in the main execution phase */
    Stages                  _onSuccessStages;       /** Stages to execute if the main execution phase completes successfully */
    Stages                  _onFailureStages;       /** Stages to execute if the main execution phase throws an exception */
    Stages                  _finalizationStages;    /** Stages to execute after the main execution phase and any success/failure stages, regardless of whether an exception was thrown or not */
    SharedWorkflowContext   _workflowContext;       /** Shared context that can be used to store and share data across stages and jobs within this workflow plan (see WorkflowContextBase for more information) */
    double                  _weight = 1.0;          /** Relative weight of this workflow plan when executed as part of a larger workflow (e.g., as a nested workflow within a job of another workflow plan). This can be used to influence progress reporting and scheduling decisions when multiple workflows are executed together. The default weight is 1.0, and it can be set to any positive value to indicate the relative importance or contribution of this workflow plan compared to others. */
    
};

}
