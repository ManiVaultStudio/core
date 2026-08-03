// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowContextBase.h"
#include "WorkflowResult.h"
#include "WorkflowJobResult.h"
#include "WorkflowHandle.h"

#include <QString>
#include <QVariant>
#include <QUuid>
#include <optional>
#include <concepts>

namespace mv
{
    class Task;
}

namespace mv::workflow
{

class AbstractWorkflowPlanExecutor;

class WorkflowExecutionContext;
class WorkflowPlan;

/** Shared reference to a workflow execution context. */
using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;

/** Shared reference to a workflow plan executor. */
using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;

/** Collection of workflow execution contexts. */
using SharedWorkflowExecutionContexts = std::vector<SharedWorkflowExecutionContext>;

/** Unique owner for a workflow plan. */
using UniqueWorkflowPlan = std::unique_ptr<WorkflowPlan>;

/**
 * @brief Describes an executable workflow.
 *
 * WorkflowPlan is a declarative description consumed by an
 * AbstractWorkflowPlanExecutor. It contains main stages, optional
 * success/failure/finalization stages, shared workflow context, and relative
 * progress weight information. Stages may execute sequentially, in parallel,
 * or in batches, and individual jobs can run normal functions or create nested
 * workflow plans.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowPlan
{
public:

    /**
     * @brief Defines how jobs inside a stage are scheduled.
     */
    enum class ConcurrencyMode
    {
        Sequential, /**< Jobs run one after another. */
        Parallel    /**< Jobs may run concurrently. */
    };

    /**
     * @brief Defines the workflow phase in which a stage participates.
     */
    enum class StageExecutionPolicy {
        Main,       /**< Main workflow execution. */
        OnSuccess,  /**< Runs after successful main execution. */
        OnFailure,  /**< Runs after failed main execution. */
        Finally     /**< Runs after main and success/failure stages. */
    };

    /**
     * @brief Defines which thread a job should run on.
     */
    enum class JobThreadAffinity {
        CurrentWorkerThread,   /**< Run on the current worker thread. */
        GuiThread              /**< Dispatch to the GUI thread. */
    };

    /**
     * @brief Defines how job completion is determined.
     */
    enum class JobCompletionPolicy {
        Auto,           /**< Completion is inferred by the executor. */
        ManualOrNested  /**< Completion is controlled manually or by nested workflow progress. */
    };

    /**
     * @brief Defines how job progress contributes to parent progress.
     */
    enum class JobProgressMode {
        Automatic,  /**< Leaf jobs auto-complete when they have no children. */
        Atomic,     /**< Nested workflow progress is treated as one job unit. */
        Nested      /**< Nested workflows contribute fine-grained progress. */
    };

    /** Shared mutable variant-map context used by workflow plans and jobs. */
    using SharedState = std::shared_ptr<QVariantMap>;

public:

    class Job;

    /** Callable job body. */
    using JobFunction = std::function<void(const Job&, const SharedWorkflowExecutionContext&)>;

    /** Collection of callable job bodies. */
    using JobFunctions = std::vector<JobFunction>;

    /** Function that creates a nested workflow plan for a job. */
    using NestedWorkflowFunction = std::function<UniqueWorkflowPlan(const Job&, const SharedWorkflowExecutionContext&)>;

    /** Function that resolves a batched stage size from an execution context. */
    using WorkflowBatchSizeFunction = std::function<std::size_t(const SharedWorkflowExecutionContext&)>;

    /** Optional batched stage size resolver. */
    using OptionalWorkflowBatchSizeFunction = std::optional<WorkflowBatchSizeFunction>;

    /**
     * @brief Wrapper used to disambiguate nested workflow jobs.
     */
    struct NestedWorkflowJob
    {
        NestedWorkflowFunction function;    /**< Function that creates the nested workflow plan. */
    };

    /**
     * @brief Smallest executable unit in a workflow.
     *
     * Jobs either wrap a function or a nested workflow factory. Each job has an
     * identifier, output identifier, thread affinity, progress mode, relative
     * weight, optional result/error state, and access to the plan's shared
     * workflow context.
     */
    class CORE_EXPORT Job
    {
    public:

        /**
         * @brief Defines the concrete kind of work represented by the job.
         */
        enum class JobKind {
            Function,       /**< Job executes a callable directly. */
            NestedWorkflow  /**< Job creates and executes a nested workflow. */
        };

    public:

        /** Error message type stored by jobs. */
        using ErrorString = QString;

        /**
         * @brief Constructs a function job.
         * @param name Human-readable job name.
         * @param function Callable executed by this job.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         */
        Job(QString name, JobFunction function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, JobProgressMode progressMode = JobProgressMode::Automatic);

        /**
         * @brief Constructs a nested workflow job.
         * @param name Human-readable job name.
         * @param nestedWorkflowFunction Function that creates the nested workflow.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         * @param weight Relative progress weight.
         */
        Job(QString name, NestedWorkflowFunction nestedWorkflowFunction, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, JobProgressMode progressMode = JobProgressMode::Automatic, double weight = 1.0);

        /**
         * @brief Constructs a nested workflow job from a wrapper.
         * @param name Human-readable job name.
         * @param job Nested workflow job descriptor.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         * @param weight Relative progress weight.
         */
        Job(QString name, NestedWorkflowJob job, JobThreadAffinity threadAffinity, JobProgressMode progressMode, double weight);

        /**
         * @brief Returns the unique job identifier.
         * @return Unique job identifier.
         */
        [[nodiscard]] QUuid getId() const { return _id; }

        /**
         * @brief Assigns the identifier used for storing this job's output.
         * @param outputId Output identifier.
         */
        void setOutputId(const QUuid& outputId);

        /**
         * @brief Returns the identifier used for storing this job's output.
         * @return Output identifier.
         */
        [[nodiscard]] QUuid getOutputId() const;

        /**
         * @brief Returns a handle that refers to this job.
         * @return Workflow handle for this job.
         */
        [[nodiscard]] WorkflowHandle getHandle() const;

        /**
         * @brief Returns the human-readable job name.
         * @return Job name.
         */
        [[nodiscard]] QString getName() const;

        /** Runs the job function in the supplied execution context. */
        void run(SharedWorkflowExecutionContext context) const;

        /** Stores a result value on the job. */
        void setResult(QVariant result);

        /** @return Current job result value. */
        [[nodiscard]] const QVariant& getResult() const;

        /** Clears the stored result value. */
        void clearResult();

        /** Stores an error message on the job. */
        void setError(QString error);

        /** @return True when the job has an error message. */
        [[nodiscard]] bool hasError() const;

        /** @return Stored error message. */
        [[nodiscard]] const QString& getError() const;

        /** Clears the stored error message. */
        void clearError();

        /** Stores an exception message on the job. */
        void setException(const std::exception& e);

        /** Stores a generic unknown-exception message on the job. */
        void setUnknownException();

        /** Marks the job as failed with an error message. */
        void fail(QString error);

        /** @return Thread affinity for this job. */
        [[nodiscard]] JobThreadAffinity getThreadAffinity() const;

        /** @return Relative progress weight for this job. */
        [[nodiscard]] double getWeight() const;

        /** Returns a copy of this job with a different relative progress weight. */
        [[nodiscard]] Job weighted(double weight);

        /** @return Progress aggregation mode for this job. */
        [[nodiscard]] JobProgressMode getProgressMode() const;

        /** @return True when this job creates a nested workflow. */
        [[nodiscard]] bool isNestedWorkflow() const
        {
            return _kind == JobKind::NestedWorkflow;
        }

        /**
         * @brief Creates the nested workflow represented by this job.
         * @param context Workflow execution context used during nested workflow creation.
         * @return Newly created nested workflow plan.
         */
        [[nodiscard]] UniqueWorkflowPlan createNestedWorkflow(const SharedWorkflowExecutionContext& context) const
        {
            Q_ASSERT(_nestedWorkflowFunction);
            return _nestedWorkflowFunction(*this, context);
        }

    public:

        /**
         * @brief Returns the shared workflow context associated with this job.
         * @return Shared workflow context.
         */
        [[nodiscard]] SharedWorkflowContext getWorkflowContext() const;

        /**
         * @brief Returns the shared workflow context cast to a specific type.
         *
         * @tparam WorkflowContextType Target workflow context type.
         * @return Context cast to WorkflowContextType, or nullptr if the cast fails.
         */
        template<typename WorkflowContextType>
        [[nodiscard]] std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const {
            static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");

            return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
        }

    protected:

        /**
         * @brief Sets the shared workflow context for this job.
         * @param workflowContext Shared workflow context to associate with this job.
         */
        void setWorkflowContext(SharedWorkflowContext workflowContext);

    private:

        QUuid                       _id = QUuid::createUuid();                                 /**< Unique job identifier. */
        QUuid                       _outputId;                                                 /**< Optional output routing identifier. */
        QString                     _name;                                                     /**< Human-readable job name. */
        JobKind                     _kind = JobKind::Function;                                 /**< Job payload type. */
        JobFunction                 _function;                                                 /**< Function job body. */
        NestedWorkflowFunction      _nestedWorkflowFunction;                                   /**< Nested workflow factory. */
        QVariant                    _result;                                                   /**< Job-local result value. */
        std::optional<QString>      _error;                                                    /**< Optional job error message. */
        JobThreadAffinity           _threadAffinity = JobThreadAffinity::CurrentWorkerThread;  /**< Required execution thread. */
        double                      _weight = 1.0;                                             /**< Relative progress weight. */
        JobProgressMode             _progressMode = JobProgressMode::Automatic;                /**< Progress aggregation mode. */
        SharedWorkflowContext       _workflowContext;                                          /**< Shared workflow context inherited from the plan. */

        friend class WorkflowPlan;
    };

    /** Collection of workflow jobs. */
    using Jobs = std::vector<Job>;

    /**
     * @brief Ordered group of jobs with a shared execution mode.
     *
     * A stage can run sequentially, in parallel, or in batches when a batch size
     * resolver is configured. Stages also carry their own handle and progress
     * weight so callers can address outputs produced by a stage.
     */
    class CORE_EXPORT Stage
    {
    public:

        /**
         * @brief Constructs a sequential or parallel stage.
         * @param name Human-readable stage name.
         * @param concurrencyMode Stage concurrency mode.
         * @param jobs Jobs contained by the stage.
         * @param weight Relative progress weight.
         */
        Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs, double weight = 1.0);

        /**
         * @brief Constructs a batched parallel stage.
         * @param name Human-readable stage name.
         * @param jobs Jobs executed in batches.
         * @param batchSizeFunction Function that resolves the batch size.
         * @param weight Relative progress weight.
         */
        Stage(QString name, Jobs jobs, WorkflowBatchSizeFunction batchSizeFunction, double weight);

        /** @return Unique stage identifier. */
        [[nodiscard]] QUuid getId() const;

        /** @return Workflow handle for this stage. */
        [[nodiscard]] WorkflowHandle getHandle() const;

        /** @return Stage concurrency mode. */
        [[nodiscard]] ConcurrencyMode getConcurrencyMode() const;

        /** @return True when the stage runs sequentially. */
        [[nodiscard]] bool isSequential() const;

        /** @return True when the stage runs in parallel. */
        [[nodiscard]] bool isParallel() const;

        /** @return True when this is a batched parallel stage. */
        [[nodiscard]] bool isBatchedParallel() const;

        /**
         * @brief Sets the batch-size resolver for a parallel stage.
         * @param batchSizeFunction Function that resolves the batch size.
         */
        void setBatchSizeFunction(WorkflowBatchSizeFunction batchSizeFunction);

        /**
         * @brief Resolves the batch size for an execution context.
         * @param executionContext Execution context used by the resolver.
         * @return Batch size to use for this stage.
         */
        [[nodiscard]] std::size_t resolveBatchSize(const SharedWorkflowExecutionContext& executionContext) const;

        /** @return Human-readable stage name. */
        [[nodiscard]] QString getName() const;

        /** @return Const jobs in this stage. */
        [[nodiscard]] const Jobs& getJobs() const;

        /** @return Mutable jobs in this stage. */
        [[nodiscard]] Jobs& getJobs();

        /** Sets the relative progress weight for this stage. */
        void setWeight(double weight);

        /** @return Relative progress weight for this stage. */
        [[nodiscard]] double getWeight() const;

        /** @return True when the stage contains GUI-thread jobs. */
        [[nodiscard]] bool containsGuiThreadJobs() const;

        /** @return True when the stage contains worker-thread jobs. */
        [[nodiscard]] bool containsWorkerThreadJobs() const;

        /** @return True when the stage's concurrency mode supports its job thread affinities. */
        [[nodiscard]] bool isThreadAffinityCompatible() const;

    private:

        QUuid                               _id = QUuid::createUuid();         /**< Unique stage identifier. */
        QString                             _name;                             /**< Human-readable stage name. */
        ConcurrencyMode                     _concurrencyMode;                  /**< Stage concurrency mode. */
        Jobs                                _jobs;                             /**< Jobs contained by this stage. */
        double                              _weight = 1.0;                     /**< Relative progress weight. */
        OptionalWorkflowBatchSizeFunction   _batchSizeFunction;                /**< Optional batch-size resolver for parallel stages. */
    };

    using Stages = std::vector<Stage>;

    /**
     * @brief Constructs a workflow plan.
     * @param title Human-readable workflow name.
     * @param context Optional shared workflow context passed to jobs.
     */
    WorkflowPlan(const QString& title, SharedWorkflowContext context = SharedWorkflowContext());

    /**
     * @brief Adds a fully constructed stage to the main workflow stage list.
     * @param stage Stage to add.
     * @return Handle for the added stage.
     */
    WorkflowHandle addStage(Stage stage);

    /**
     * @brief Adds a single-function sequential stage.
     * @param name Stage and job name.
     * @param function Function to execute.
     * @param threadAffinity Thread affinity for the generated job.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    template<typename Function>
    WorkflowHandle addSequentialStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 1.0) {
        return addStageTo(_stages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /** @return Human-readable workflow plan name. */
    [[nodiscard]] QString getName() const;

    /**
     * @brief Adds a sequential stage to the main workflow stage list.
     * @param name Human-readable stage name.
     * @param jobs Jobs contained by the stage.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    WorkflowHandle addSequentialStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a parallel stage to the main workflow stage list.
     * @param name Human-readable stage name.
     * @param jobs Jobs contained by the stage.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    WorkflowHandle addParallelStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a parallel stage containing only nested workflow jobs.
     * @param name Human-readable stage name.
     * @param jobs Nested workflow jobs contained by the stage.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    WorkflowHandle addParallelNestedWorkflowStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a batched parallel stage with a fixed batch size.
     * @param name Stage name.
     * @param jobs Jobs to execute in batches.
     * @param batchSize Fixed batch size.
     * @return Handle for the added stage.
     */
    WorkflowHandle addBatchedParallelStage(const QString& name, Jobs jobs, std::size_t batchSize);

    /**
     * @brief Adds a batched parallel stage with a dynamic batch-size resolver.
     * @param name Stage name.
     * @param jobs Jobs to execute in batches.
     * @param batchSizeFunction Function that resolves the batch size.
     * @return Handle for the added stage.
     */
    WorkflowHandle addBatchedParallelStage(const QString& name, Jobs jobs, WorkflowBatchSizeFunction batchSizeFunction);

    /**
     * @brief Adds a stage using an explicit concurrency mode.
     * @param name Human-readable stage name.
     * @param mode Stage concurrency mode.
     * @param jobs Jobs contained by the stage.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    WorkflowHandle addStage(QString name, ConcurrencyMode mode, Jobs jobs, double weight = 1.0);

    /** Adds a single-function stage that runs when the main workflow succeeds. */
    template<typename Function>
    void addOnSuccessStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_onSuccessStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /** Adds a single-function stage that runs when the main workflow fails. */
    template<typename Function>
    void addOnFailureStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_onFailureStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /** Adds a single-function stage that runs during finalization. */
    template<typename Function>
    void addFinalizationStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0) {
        addStageTo(_finalizationStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /** @return Const main workflow stages. */
    [[nodiscard]] const Stages& getStages() const;

    /** @return Mutable main workflow stages. */
    [[nodiscard]] Stages& getStages();

    /** @return Stages that execute after successful main execution. */
    [[nodiscard]] const Stages& getOnSuccessStages() const;

    /** @return Stages that execute after failed main execution. */
    [[nodiscard]] const Stages& getOnFailureStages() const;

    /** @return Stages that execute during finalization. */
    [[nodiscard]] const Stages& getFinallyStages() const;

    /**
     * @brief Gets the shared workflow context.
     *
     * The workflow context can store data shared across stages and jobs in
     * this plan.
     *
     * @return Shared workflow context.
     */
    [[nodiscard]] SharedWorkflowContext getWorkflowContext() const;

    /**
     * @brief Returns the shared workflow context cast to a specific type.
     *
     * WorkflowContextType must derive from WorkflowContextBase. The method
     * returns nullptr when the stored context has a different type.
     *
     * @tparam WorkflowContextType Target workflow context type.
     * @return Context cast to WorkflowContextType, or nullptr if the cast fails.
     */
    template<typename WorkflowContextType>
    [[nodiscard]] std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const {
        static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");
        return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
    }

    /** Sets the relative progress weight for this workflow plan. */
    void setWeight(double weight);

    /** @return Relative progress weight for this workflow plan. */
    [[nodiscard]] double getWeight() const;

    /**
     * @brief Adds a sequential stage that executes a nested workflow.
     * @param name Stage and nested workflow job name.
     * @param function Function that creates the nested workflow plan.
     * @param threadAffinity Thread affinity for the nested workflow job.
     * @param weight Relative progress weight.
     * @return Handle for the added nested workflow stage.
     */
    WorkflowHandle addNestedWorkflowStage(const QString& name, NestedWorkflowFunction function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 1.0);

private:

    /**
     * @brief Invokes a job function using one of the supported function signatures.
     * @param function Function object to invoke.
     * @param job Job being executed.
     * @param context Execution context for the job.
     */
    template<typename Function>
    static void invokeJobFunction(Function& function, const Job& job, const SharedWorkflowExecutionContext& context)
    {
        if constexpr (std::is_invocable_v<Function, const Job&, const SharedWorkflowExecutionContext&>) {
            function(job, context);
        }
        else if constexpr (std::is_invocable_v<Function, Job&, const SharedWorkflowExecutionContext&>) {
            auto mutableJob = job;
            function(mutableJob, context);
        }
        else if constexpr (std::is_invocable_v<Function, const SharedWorkflowExecutionContext&>) {
            function(context);
        }
        else if constexpr (std::is_invocable_v<Function>) {
            function();
        }
        else {
            static_assert(std::is_invocable_v<Function>,
                "Stage function must be callable as one of: "
                "void(const Job&, const SharedWorkflowExecutionContext&), "
                "void(Job&, const SharedWorkflowExecutionContext&), "
                "void(const SharedWorkflowExecutionContext&), "
                "or void()");
        }
    }

    /**
     * @brief Adds a single-function stage to a stage collection.
     * @param stages Target stage collection.
     * @param name Stage and job name.
     * @param function Function to wrap as a job.
     * @param threadAffinity Thread affinity for the generated job.
     * @param weight Relative progress weight.
     * @return Handle for the added stage.
     */
    template<typename Function>
    WorkflowHandle addStageTo(Stages& stages, QString name, Function&& function, JobThreadAffinity threadAffinity, double weight)
    {
        if constexpr (
            std::is_invocable_v<Function, const Job&, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function, Job&, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function>) {
            const auto jobName = name;

            stages.emplace_back(std::move(name), ConcurrencyMode::Sequential, Jobs{ Job(jobName, [fn = std::forward<Function>(function)](const Job& job, const SharedWorkflowExecutionContext& context) mutable {
                invokeJobFunction(fn, job, context);
            }, threadAffinity)}, weight);
        }
        else {
            static_assert(std::is_invocable_v<Function>,
                "Unsupported workflow stage function signature. Supported signatures are "
                "void(const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&), "
                "void(WorkflowPlan::Job&, const SharedWorkflowExecutionContext&), "
                "void(const SharedWorkflowExecutionContext&), or void().");
        }

        return stages.back().getHandle();
    }

private:
    QString                 _name;                  /**< Human-readable workflow plan name. */
    Stages                  _stages;                /**< Main workflow stages. */
    Stages                  _onSuccessStages;       /**< Stages executed after successful main execution. */
    Stages                  _onFailureStages;       /**< Stages executed after failed main execution. */
    Stages                  _finalizationStages;    /**< Stages executed during finalization. */
    SharedWorkflowContext   _workflowContext;       /**< Shared workflow context passed to stages and jobs. */
    double                  _weight = 1.0;          /**< Relative progress weight when this plan is nested. */
    
};

}
