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
#include <QJsonDocument>
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

    enum class JobThreadAffinity {
        CurrentWorkerThread,
        GuiThread
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

        Job(QString name, JobFunction function, JobThreadAffinity threadAffinity = JobThreadAffinity::CurrentWorkerThread);
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

    private:
        QString     _name;
        JobFunction _function;
        QVariant    _result;
        std::optional<QString> _error;
        JobThreadAffinity _threadAffinity = JobThreadAffinity::CurrentWorkerThread;
        double          _weight = 1.0;
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
        if constexpr (std::is_invocable_v<Function, Job&>) {
            _stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, { Job(name, JobFunction(std::forward<Function>(function)), threadAffinity) }, weight));
        }
        else if constexpr (std::is_invocable_v<Function>) {
            _stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, { Job(name, JobFunction([fn = std::forward<Function>(function)](Job&) mutable { fn(); }), threadAffinity) }, weight));
        }
        else {
            static_assert(std::is_invocable_v<Function, Job&> || std::is_invocable_v<Function>, "Stage function must be callable as void(Job&) or void()");
        }
    }

    QString getName() const;

    void    addSequentialStage(QString name, Jobs jobs, double weight = 1.0);
	void    addParallelStage(QString name, Jobs jobs, double weight = 1.0);
    void    addStage(QString name, ConcurrencyMode mode, Jobs jobs, double weight = 1.0);

    WorkflowResult execute(const SharedWorkflowPlanExecutor& workflowPlanExecutor, bool showProgress = false, WorkflowExecutionOptions executionOptions = {});
    WorkflowResultFuture executeAsync(const SharedWorkflowPlanExecutor& workflowPlanExecutor, bool showProgress = false, WorkflowExecutionOptions executionOptions = {});
    WorkflowResult executeOnCurrentThread(const SharedWorkflowPlanExecutor& workflowPlanExecutor, Task* task = nullptr, WorkflowExecutionOptions executionOptions = {});
    Stages                getStages() const;

    SharedState           getSharedState() const;

    SharedWorkflowContext getWorkflowContext() const;

    template<typename WorkflowContextType>
    std::shared_ptr<WorkflowContextType> getWorkflowContextAs() const {
        static_assert(std::derived_from<WorkflowContextType, WorkflowContextBase>, "WorkflowContextType must derive from WorkflowContextBase");
        return std::dynamic_pointer_cast<WorkflowContextType>(_workflowContext);
    }

    void setWeight(double weight);

    double getWeight() const;

private:
    QString _name;
    Stages  _stages;
    SharedState _sharedState = std::make_shared<QVariantMap>();
    SharedWorkflowContext   _workflowContext;
    double _weight = 1.0;
};

}
