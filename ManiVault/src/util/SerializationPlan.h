// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "AbstractSerializationPlanExecutor.h"

#include <QString>
#include <QJsonDocument>

namespace mv::util
{

class AbstractSerializationPlanExecutor;

class CORE_EXPORT SerializationPlan
{

public:
    class Job;

    using JobFunction   = std::function<void(Job&)>;
    using JobFunctions  = std::vector<JobFunction>;

    class CORE_EXPORT Job
    {
    public:
        Job(QString name, JobFunction function);

        QString getName() const { return _name; }
        JobFunction getFunction() const { return _function; }

        void run()
        {
            if (_function)
                _function(*this);
        }

        void setResult(QVariant result) { _result = std::move(result); }
        const QVariant& getResult() const { return _result; }

    private:
        QString         _name;
        JobFunction     _function;
        QVariant        _result;
    };

    using Jobs = std::vector<Job>;

    class CORE_EXPORT Stage
    {
    public:
        Stage(QString name, Jobs jobs);

        QString getName() const { return _name; }
        Jobs getJobs() const { return _jobs; }

    private:
        QString _name;
        Jobs    _jobs;
    };

    using Stages = std::vector<Stage>;

    void addStage(Stage stage);
	void addSequentialStage(QString name, JobFunction jobFunction);
	void addParallelStage(QString name, Jobs jobs);

	void execute(AbstractSerializationPlanExecutor& serializationPlanExecutor);

    QVariant getResult() const { return _result; }

private:
    Stages  _stages;
    QVariant _result;
};

}