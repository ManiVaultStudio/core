// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractWorkflowPlanExecutor.h"

#include <QString>
#include <QJsonDocument>

namespace mv::util
{

class AbstractWorkflowPlanExecutor;

class CORE_EXPORT WorkflowPlan
{
public:

    enum class ConcurrencyMode
    {
        Sequential,
        Parallel
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

        Job(QString name, JobFunction function);

        QString getName() const;

        const JobFunction& getFunction() const;

        void run();

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

    private:
        QString     _name;
        JobFunction _function;
        QVariant    _result;
        std::optional<QString> _error;
    };

    using Jobs = std::vector<Job>;

    class CORE_EXPORT Stage
    {
    public:

        Stage(QString name, ConcurrencyMode concurrencyMode, Jobs jobs);

        ConcurrencyMode getMode() const;

        QString getName() const;

        Jobs    getJobs() const;

    private:
        QString         _name;
        ConcurrencyMode _concurrencyMode;
        Jobs            _jobs;
    };

    using Stages = std::vector<Stage>;

    void addStage(Stage stage);

    template<typename Function>
    void addSequentialStage(QString name, Function&& function)
    {
        if constexpr (std::is_invocable_v<Function, Job&>) {
            _stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, {
				Job(std::move(name), JobFunction(std::forward<Function>(function)))
            }));
        }
        else if constexpr (std::is_invocable_v<Function>) {
            _stages.emplace_back(Stage(name, ConcurrencyMode::Sequential, {
                Job(std::move(name), JobFunction([fn = std::forward<Function>(function)](Job&) mutable {
	                fn();
	            }))
            }));
        }
        else {
            static_assert(std::is_invocable_v<Function, Job&> || std::is_invocable_v<Function>,
                "Stage function must be callable as void(Job&) or void()");
        }
    }

    void addSequentialStage(QString name, Jobs jobs);
	void addParallelStage(QString name, Jobs jobs);
    void addStage(QString name, ConcurrencyMode mode, Jobs jobs);

	void execute(AbstractWorkflowPlanExecutor& WorkflowPlanExecutor);

    Stages getStages() const { return _stages; }
    SharedState getSharedState() const { return _sharedState; }

private:
    Stages  _stages;
    SharedState _sharedState = std::make_shared<QVariantMap>();
};

}