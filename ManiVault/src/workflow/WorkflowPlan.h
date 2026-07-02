// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowContextBase.h"
#include "WorkflowResult.h"
#include "WorkflowResultFuture.h"
#include "WorkflowExecutionOptions.h"
#include "WorkflowHandle.h"

#include <QString>
#include <QVariant>

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

using SharedWorkflowExecutionContext = std::shared_ptr<WorkflowExecutionContext>;
using SharedWorkflowPlanExecutor = std::shared_ptr<AbstractWorkflowPlanExecutor>;
using SharedWorkflowExecutionContexts = std::vector<SharedWorkflowExecutionContext>;
using UniqueWorkflowPlan = std::unique_ptr<WorkflowPlan>;

/**
 * @brief Declarative description of an executable workflow.
 *
 * A workflow plan consists of one or more stages. Each stage contains one or
 * more jobs and defines how those jobs should be executed. Stages may execute
 * sequentially, in parallel, in batches, or by constructing nested workflows.
 *
 * WorkflowPlan does not execute work by itself. It describes the work to be
 * performed and is consumed by an AbstractWorkflowPlanExecutor, which translates
 * the plan into an executable schedule.
 *
 * A workflow plan may contain main stages, success stages, failure stages, and
 * finalization stages. Main stages define the primary workflow. Success stages
 * run only after successful completion, failure stages run only when execution
 * fails, and finalization stages run regardless of outcome.
 *
 * Workflow plans may also carry a shared workflow context. This context allows
 * jobs and stages to share state without explicit parameter passing.
 */
class CORE_EXPORT WorkflowPlan
{
public:

    /**
     * @brief Defines how jobs inside a stage are scheduled.
     */
    enum class ConcurrencyMode
    {
        Sequential,     /**< Jobs are executed one after another. */
        Parallel        /**< Jobs may be executed concurrently. */
    };

    /**
     * @brief Defines the phase in which a stage participates.
     */
    enum class StageExecutionPolicy
    {
        Main,           /**< Stage belongs to the primary workflow. */
        OnSuccess,      /**< Stage runs only if the main workflow succeeds. */
        OnFailure,      /**< Stage runs only if the main workflow fails. */
        Finally         /**< Stage runs regardless of success or failure. */
    };

    /**
     * @brief Defines the thread affinity of a job.
     */
    enum class JobThreadAffinity
    {
        CurrentWorkerThread,    /**< Job runs on the current executor worker thread. */
        GuiThread               /**< Job runs on the GUI thread. */
    };

    /**
     * @brief Defines how job completion is managed.
     */
    enum class JobCompletionPolicy
    {
        Auto,                   /**< Job completes automatically when its function returns. */
        ManualOrNested          /**< Job completion is controlled manually or by a nested workflow. */
    };

    /**
     * @brief Defines how a job contributes to workflow progress.
     */
    enum class JobProgressMode
    {
        Automatic,  /**< Leaf jobs complete automatically when their function returns. */
        Atomic,     /**< Job is treated as one indivisible progress unit. */
        Nested      /**< Nested workflow progress contributes to this job. */
    };

    using SharedState = std::shared_ptr<QVariantMap>;

public:
    class Job;

    using JobFunction = std::function<void(const Job&, const SharedWorkflowExecutionContext&)>;
    using JobFunctions = std::vector<JobFunction>;
    using NestedWorkflowFunction = std::function<UniqueWorkflowPlan(const Job&, const SharedWorkflowExecutionContext&)>;
    using WorkflowBatchSizeFunction = std::function<std::size_t(const SharedWorkflowExecutionContext&)>;
    using OptionalWorkflowBatchSizeFunction = std::optional<WorkflowBatchSizeFunction>;

    /**
     * @brief Wrapper for a nested workflow factory.
     */
    struct NestedWorkflowJob
    {
        NestedWorkflowFunction function;    /**< Function that creates the nested workflow plan. */
    };

    /**
     * @brief Smallest executable unit in a workflow.
     *
     * A job either wraps a regular function or a nested workflow factory. Jobs
     * are grouped into stages and executed by the workflow executor according to
     * the stage concurrency mode and the job thread affinity.
     *
     * A job may produce a result, report an error, carry a workflow context, and
     * contribute to progress reporting through its configured weight and progress
     * mode.
     */
    class CORE_EXPORT Job
    {
    public:

        /**
         * @brief Defines the concrete kind of work represented by the job.
         */
        enum class JobKind
        {
            Function,           /**< Job executes a callable directly. */
            NestedWorkflow      /**< Job creates and executes a nested workflow. */
        };

    public:
        using ErrorString = QString;

        /**
         * @brief Constructs a function job.
         *
         * @param name Human-readable job name.
         * @param function Callable executed by this job.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         */
        Job(QString name,
            JobFunction function,
            JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread,
            JobProgressMode progressMode = JobProgressMode::Automatic);

        /**
         * @brief Constructs a nested workflow job.
         *
         * @param name Human-readable job name.
         * @param nestedWorkflowFunction Function that creates the nested workflow.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         * @param weight Relative progress weight.
         */
        Job(QString name,
            NestedWorkflowFunction nestedWorkflowFunction,
            JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread,
            JobProgressMode progressMode = JobProgressMode::Automatic,
            double weight = 1.0);

        /**
         * @brief Constructs a nested workflow job from a NestedWorkflowJob wrapper.
         *
         * @param name Human-readable job name.
         * @param job Nested workflow job descriptor.
         * @param threadAffinity Preferred execution thread.
         * @param progressMode Progress reporting mode for this job.
         * @param weight Relative progress weight.
         */
        Job(QString name,
            NestedWorkflowJob job,
            JobThreadAffinity threadAffinity,
            JobProgressMode progressMode,
            double weight);

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

        /**
         * @brief Executes the job using the provided execution context.
         * @param context Workflow execution context.
         */
        void run(SharedWorkflowExecutionContext context) const;

        /**
         * @brief Stores the job result.
         * @param result Result value produced by the job.
         */
        void setResult(QVariant result);

        /**
         * @brief Returns the job result.
         * @return Stored job result.
         */
        [[nodiscard]] const QVariant& getResult() const;

        /**
         * @brief Clears the stored job result.
         */
        void clearResult();

        /**
         * @brief Stores an error message for this job.
         * @param error Error message.
         */
        void setError(QString error);

        /**
         * @brief Returns whether this job has an error.
         * @return True if an error is stored, otherwise false.
         */
        [[nodiscard]] bool hasError() const;

        /**
         * @brief Returns the stored error message.
         * @return Error message.
         */
        [[nodiscard]] const QString& getError() const;

        /**
         * @brief Clears the stored error message.
         */
        void clearError();

        /**
         * @brief Stores an error message derived from an exception.
         * @param e Exception to convert to an error message.
         */
        void setException(const std::exception& e);

        /**
         * @brief Stores a generic error message for an unknown exception.
         */
        void setUnknownException();

        /**
         * @brief Marks the job as failed.
         * @param error Error message describing the failure.
         */
        void fail(QString error);

        /**
         * @brief Returns the preferred execution thread for this job.
         * @return Job thread affinity.
         */
        [[nodiscard]] JobThreadAffinity getThreadAffinity() const;

        /**
         * @brief Returns the relative progress weight of this job.
         * @return Progress weight.
         */
        [[nodiscard]] double getWeight() const;

        /**
         * @brief Returns a copy of this job with a different weight.
         * @param weight New relative progress weight.
         * @return Weighted job copy.
         */
        [[nodiscard]] Job weighted(double weight);

        /**
         * @brief Returns the progress reporting mode of this job.
         * @return Job progress mode.
         */
        [[nodiscard]] JobProgressMode getProgressMode() const;

        /**
         * @brief Returns whether this job represents a nested workflow.
         * @return True if this is a nested workflow job, otherwise false.
         */
        [[nodiscard]] bool isNestedWorkflow() const
        {
            return _kind == JobKind::NestedWorkflow;
        }

        /**
         * @brief Creates the nested workflow represented by this job.
         *
         * This function is valid only for jobs whose kind is NestedWorkflow.
         *
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
        [[nodiscard]] std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const
        {
            static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>,
                "WorkflowContextType must derive from WorkflowContextBase");

            return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
        }

    protected:

        /**
         * @brief Assigns the shared workflow context associated with this job.
         * @param workflowContext Shared workflow context.
         */
        void setWorkflowContext(SharedWorkflowContext workflowContext);

    private:
        QUuid                       _id = QUuid::createUuid();              /**< Unique job identifier. */
        QUuid                       _outputId;                              /**< Identifier used for storing job output. */
        QString                     _name;                                  /**< Human-readable job name. */
        JobKind                     _kind = JobKind::Function;              /**< Type of job represented by this object. */
        JobFunction                 _function;                              /**< Callable executed by function jobs. */
        NestedWorkflowFunction      _nestedWorkflowFunction;                /**< Factory used by nested workflow jobs. */
        QVariant                    _result;                                /**< Optional result produced by this job. */
        std::optional<QString>      _error;                                 /**< Optional error message. */
        JobThreadAffinity           _threadAffinity = JobThreadAffinity::CurrentWorkerThread; /**< Preferred execution thread. */
        double                      _weight = 1.0;                          /**< Relative progress weight. */
        JobProgressMode             _progressMode = JobProgressMode::Automatic; /**< Progress reporting mode. */
        SharedWorkflowContext       _workflowContext;                       /**< Shared workflow context visible to this job. */

        friend class WorkflowPlan;
    };

    using Jobs = std::vector<Job>;

    /**
     * @brief Group of jobs that execute as one workflow step.
     *
     * A stage is a synchronization boundary in a workflow. The executor starts
     * the next stage only after the current stage has completed. Jobs inside a
     * stage may execute sequentially, in parallel, or in batches depending on
     * the stage configuration.
     */
    class CORE_EXPORT Stage
    {
    public:

        /**
         * @brief Constructs a regular stage.
         *
         * @param name Human-readable stage name.
         * @param concurrencyMode Execution mode for jobs in this stage.
         * @param jobs Jobs belonging to this stage.
         * @param weight Relative progress weight.
         */
        Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs, double weight = 1.0);

        /**
         * @brief Constructs a batched parallel stage.
         *
         * @param name Human-readable stage name.
         * @param jobs Jobs belonging to this stage.
         * @param batchSizeFunction Function used to resolve the batch size.
         * @param weight Relative progress weight.
         */
        Stage(QString name, Jobs jobs, WorkflowBatchSizeFunction batchSizeFunction, double weight);

        /**
         * @brief Returns the unique stage identifier.
         * @return Unique stage identifier.
         */
        [[nodiscard]] QUuid getId() const;

        /**
         * @brief Returns a handle that refers to this stage.
         * @return Workflow handle for this stage.
         */
        [[nodiscard]] WorkflowHandle getHandle() const;

        /**
         * @brief Returns the concurrency mode of this stage.
         * @return Stage concurrency mode.
         */
        [[nodiscard]] ConcurrencyMode getConcurrencyMode() const;

        /**
         * @brief Returns whether jobs in this stage execute sequentially.
         * @return True if this is a sequential stage, otherwise false.
         */
        [[nodiscard]] bool isSequential() const;

        /**
         * @brief Returns whether jobs in this stage execute in parallel.
         * @return True if this is a parallel stage, otherwise false.
         */
        [[nodiscard]] bool isParallel() const;

        /**
         * @brief Returns whether this stage executes parallel jobs in batches.
         * @return True if a batch size function is configured, otherwise false.
         */
        [[nodiscard]] bool isBatchedParallel() const;

        /**
         * @brief Sets the function used to resolve the batch size.
         * @param batchSizeFunction Batch size resolver.
         */
        void setBatchSizeFunction(WorkflowBatchSizeFunction batchSizeFunction);

        /**
         * @brief Resolves the batch size for this stage.
         * @param executionContext Execution context used by the batch size function.
         * @return Batch size.
         */
        [[nodiscard]] std::size_t resolveBatchSize(const SharedWorkflowExecutionContext& executionContext) const;

        /**
         * @brief Returns the human-readable stage name.
         * @return Stage name.
         */
        [[nodiscard]] QString getName() const;

        /**
         * @brief Returns the jobs in this stage.
         * @return Const reference to the stage jobs.
         */
        [[nodiscard]] const Jobs& getJobs() const;

        /**
         * @brief Returns the jobs in this stage.
         * @return Mutable reference to the stage jobs.
         */
        [[nodiscard]] Jobs& getJobs();

        /**
         * @brief Sets the relative progress weight of this stage.
         * @param weight Progress weight.
         */
        void setWeight(double weight);

        /**
         * @brief Returns the relative progress weight of this stage.
         * @return Progress weight.
         */
        [[nodiscard]] double getWeight() const;

        /**
         * @brief Returns whether this stage contains jobs that must run on the GUI thread.
         * @return True if at least one job has GUI thread affinity.
         */
        [[nodiscard]] bool containsGuiThreadJobs() const;

        /**
         * @brief Returns whether this stage contains jobs that run on worker threads.
         * @return True if at least one job has worker thread affinity.
         */
        [[nodiscard]] bool containsWorkerThreadJobs() const;

        /**
         * @brief Returns whether all jobs in this stage can be scheduled safely together.
         * @return True if the stage's thread affinities are compatible with its execution mode.
         */
        [[nodiscard]] bool isThreadAffinityCompatible() const;

    private:
        QUuid                               _id = QUuid::createUuid();      /**< Unique stage identifier. */
        QString                             _name;                          /**< Human-readable stage name. */
        ConcurrencyMode                     _concurrencyMode;               /**< Job scheduling mode. */
        Jobs                                _jobs;                          /**< Jobs belonging to this stage. */
        double                              _weight = 1.0;                  /**< Relative progress weight. */
        OptionalWorkflowBatchSizeFunction   _batchSizeFunction;             /**< Optional dynamic batch size resolver. */
    };

    using Stages = std::vector<Stage>;

    /**
     * @brief Constructs a workflow plan.
     *
     * @param title Human-readable workflow name.
     * @param context Optional shared workflow context.
     */
    WorkflowPlan(const QString& title, SharedWorkflowContext context = SharedWorkflowContext());

    /**
     * @brief Adds a stage to the main workflow.
     * @param stage Stage to add.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addStage(Stage stage);

    /**
     * @brief Adds a sequential stage consisting of a single callable.
     *
     * The callable may be invocable as void(const Job&, const SharedWorkflowExecutionContext&),
     * void(Job&, const SharedWorkflowExecutionContext&), void(const SharedWorkflowExecutionContext&),
     * or void().
     *
     * @param name Human-readable stage name.
     * @param function Callable executed by the stage.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    template<typename Function>
    WorkflowHandle addSequentialStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 1.0)
    {
        return addStageTo(_stages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /**
     * @brief Returns the human-readable workflow name.
     * @return Workflow name.
     */
    [[nodiscard]] QString getName() const;

    /**
     * @brief Adds a sequential stage to the main workflow.
     * @param name Human-readable stage name.
     * @param jobs Jobs executed by the stage.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addSequentialStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a parallel stage to the main workflow.
     * @param name Human-readable stage name.
     * @param jobs Jobs executed by the stage.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addParallelStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a parallel stage containing nested workflow jobs.
     * @param name Human-readable stage name.
     * @param jobs Nested workflow jobs executed by the stage.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addParallelNestedWorkflowStage(QString name, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a batched parallel stage with a fixed batch size.
     *
     * @param name Human-readable stage name.
     * @param jobs Jobs executed by the stage.
     * @param batchSize Maximum number of jobs executed per batch.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addBatchedParallelStage(const QString& name, Jobs jobs, std::size_t batchSize);

    /**
     * @brief Adds a batched parallel stage with a dynamic batch size.
     *
     * @param name Human-readable stage name.
     * @param jobs Jobs executed by the stage.
     * @param batchSizeFunction Function used to resolve the batch size at execution time.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addBatchedParallelStage(const QString& name, Jobs jobs, WorkflowBatchSizeFunction batchSizeFunction);

    /**
     * @brief Adds a stage to the main workflow.
     * @param name Human-readable stage name.
     * @param mode Stage concurrency mode.
     * @param jobs Jobs executed by the stage.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addStage(QString name, ConcurrencyMode mode, Jobs jobs, double weight = 1.0);

    /**
     * @brief Adds a stage that runs only after successful main workflow completion.
     *
     * @param name Human-readable stage name.
     * @param function Callable executed by the stage.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     */
    template<typename Function>
    void addOnSuccessStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0)
    {
        addStageTo(_onSuccessStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /**
     * @brief Adds a stage that runs only if the main workflow fails.
     *
     * @param name Human-readable stage name.
     * @param function Callable executed by the stage.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     */
    template<typename Function>
    void addOnFailureStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0)
    {
        addStageTo(_onFailureStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /**
     * @brief Adds a stage that always runs after workflow completion.
     *
     * @param name Human-readable stage name.
     * @param function Callable executed by the stage.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     */
    template<typename Function>
    void addFinalizationStage(QString name, Function&& function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread, double weight = 0.0)
    {
        addStageTo(_finalizationStages, std::move(name), std::forward<Function>(function), threadAffinity, weight);
    }

    /**
     * @brief Returns the main workflow stages.
     * @return Const reference to the main stages.
     */
    [[nodiscard]] const Stages& getStages() const;

    /**
     * @brief Returns the main workflow stages.
     * @return Mutable reference to the main stages.
     */
    [[nodiscard]] Stages& getStages();

    /**
     * @brief Returns the stages executed after successful completion.
     * @return Success stages.
     */
    [[nodiscard]] const Stages& getOnSuccessStages() const;

    /**
     * @brief Returns the stages executed after failure.
     * @return Failure stages.
     */
    [[nodiscard]] const Stages& getOnFailureStages() const;

    /**
     * @brief Returns the stages executed regardless of outcome.
     * @return Finalization stages.
     */
    [[nodiscard]] const Stages& getFinallyStages() const;

    /**
     * @brief Returns the shared workflow context associated with this plan.
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
    [[nodiscard]] std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const
    {
        static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");

        return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
    }

    /**
     * @brief Sets the relative progress weight of this workflow.
     * @param weight Progress weight.
     */
    void setWeight(double weight);

    /**
     * @brief Returns the relative progress weight of this workflow.
     * @return Progress weight.
     */
    [[nodiscard]] double getWeight() const;

public:

    /**
     * @brief Adds a stage that creates and executes a nested workflow.
     *
     * The nested workflow is created lazily when the stage executes. This allows
     * workflow plans to compose other workflow plans without constructing the
     * full execution hierarchy up front.
     *
     * @param name Human-readable stage name.
     * @param function Function that creates the nested workflow.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    WorkflowHandle addNestedWorkflowStage(const QString& name,
        NestedWorkflowFunction function,
        JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread,
        double weight = 1.0);

private:

    /**
     * @brief Invokes a workflow job callable using one of the supported signatures.
     *
     * This helper allows stage callables to be written with different levels of
     * access to the job and execution context while preserving a single internal
     * job execution representation.
     *
     * @tparam Function Callable type.
     * @param function Callable to invoke.
     * @param job Job being executed.
     * @param context Workflow execution context.
     */
    template<typename Function>
    static void invokeJobFunction(Function& function,
        const Job& job,
        const SharedWorkflowExecutionContext& context)
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
     * @brief Adds a single-callable sequential stage to a stage collection.
     *
     * @tparam Function Callable type.
     * @param stages Destination stage collection.
     * @param name Human-readable stage name.
     * @param function Callable executed by the stage.
     * @param threadAffinity Preferred execution thread.
     * @param weight Relative progress weight.
     * @return Handle identifying the added stage.
     */
    template<typename Function>
    [[nodiscard]] WorkflowHandle addStageTo(Stages& stages,
        QString name,
        Function&& function,
        JobThreadAffinity threadAffinity,
        double weight)
    {
        if constexpr (std::is_invocable_r_v<WorkflowResultFuture, Function, const Job&, const SharedWorkflowExecutionContext&>) {
            stages.emplace_back(name, ConcurrencyMode::Sequential, Jobs{
                Job(name, AsyncJobFunction(std::forward<Function>(function)), threadAffinity)
                }, weight);
        }
        else if constexpr (
            std::is_invocable_v<Function, const Job&, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function, Job&, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function, const SharedWorkflowExecutionContext&> ||
            std::is_invocable_v<Function>) {

            const auto jobName = name;

            stages.emplace_back(std::move(name), ConcurrencyMode::Sequential, Jobs{
                Job(jobName,
                    [fn = std::forward<Function>(function)](const Job& job, const SharedWorkflowExecutionContext& context) mutable {
                        invokeJobFunction(fn, job, context);
                    },
                    threadAffinity)
                }, weight);
        }
        else {
            static_assert(std::is_invocable_v<Function>, "Unsupported workflow stage function signature");
        }

        return stages.back().getHandle();
    }

private:
    QString                 _name;                  /**< Human-readable workflow name. */
    Stages                  _stages;                /**< Stages executed during the main workflow phase. */
    Stages                  _onSuccessStages;       /**< Stages executed after successful main workflow completion. */
    Stages                  _onFailureStages;       /**< Stages executed after main workflow failure. */
    Stages                  _finalizationStages;    /**< Stages executed regardless of workflow outcome. */
    SharedWorkflowContext   _workflowContext;       /**< Shared context available to jobs and stages. */
    double                  _weight = 1.0;          /**< Relative progress weight of this workflow. */
};

}