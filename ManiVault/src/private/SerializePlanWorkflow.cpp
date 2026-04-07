// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SerializePlanWorkflow.h"
#include "ProjectSaveResult.h"
#include "ProjectManager.h"
#include "Archiver.h"

#include <util/OperationContextScope.h>

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
	#define SERIALIZE_PLAN_WORKFLOW_VERBOSE
#endif

#define SERIALIZE_PLAN_WORKFLOW_VERBOSE

SerializePlanWorkflow::SerializePlanWorkflow(SerializationPlan serializationPlan, QObject* parent, SharedOperationContext operationContext /*= {}*/) :
    AbstractWorkflow(createContext(serializationPlan), "Serialize Plan", parent, operationContext),
    _serializationPlan(std::move(serializationPlan))
{
}

Group SerializePlanWorkflow::makeRecipe()
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Workflow", "Make recipe", 1);
#endif

    auto& contextStorage = this->contextStorage();

    GroupItems root{ sequential };

    root << contextStorage;

    for (auto& stage : _serializationPlan.getStages()) {
        root << buildStageGroup(stage);
    }

    root << onGroupDone([](DoneWith doneWith) {}, CallDoneFlag::Always);

    return root;
}

void SerializePlanWorkflow::setupStorage(WorkflowRuntimeContext& context)
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Workflow", "Setup storage", 1);
#endif
}

void SerializePlanWorkflow::onStorageDone(const WorkflowRuntimeContext& context)
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Workflow", "Storage done", 1);
#endif
}

void SerializePlanWorkflow::handleDone(QtTaskTree::DoneWith status)
{
    AbstractWorkflow::handleDone(status);
    
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Workflow", "Handle done", 1);
#endif

    const auto result = resultAs<ProjectSaveResult>();

    if (result->_success)
        emit finished(status == DoneWith::Success, QString{});
    else
        emit finished(status == DoneWith::Error, result->_errorMessage);

    //if (const auto result = resultAs<ProjectSaveResult>()) {
    //    const auto duration = getDuration();
    //    const auto text     = (duration < 1000) ? QString("%1 saved successfully in %2 ms").arg(result->_filePath).arg(duration) : QString("%1 saved successfully in %2 s").arg(result->_filePath).arg(duration / 1000.0, 0, 'f', 1);

    //    if (status == DoneWith::Success)
    //        help().addNotification("Project saved", text, StyledIcon("floppy-disk"));
    //    else
    //        help().addNotification("Error", "Unable to save ManiVault project: " + result->_errorMessage, StyledIcon("exclamation-triangle"));
    //} else {
    //    throw std::runtime_error("Unexpected error: ProjectSaveResult is null");
    //}

    
}

void SerializePlanWorkflow::initResult(UniqueWorkflowResultBase& result)
{
    result.reset(new ProjectSaveResult());
}

UniqueWorkflowContext SerializePlanWorkflow::createContext(SerializationPlan serializationPlan)
{
    auto context = std::make_unique<SerializePlanContext>();

    return context;
}

Group SerializePlanWorkflow::buildStageGroup(SerializationPlan::Stage& stage)
{
    printLine("Build stage group", stage.getName(), 2);

    GroupItems items;

    items << (stage.getMode() == SerializationPlan::Stage::Mode::Parallel ? parallel : sequential);

    for (auto& job : stage.getJobs())
        items << buildJobTask(job);

    return { items };
}

GroupItem SerializePlanWorkflow::buildJobTask(SerializationPlan::Job& job)
{
    const QString name = job.getName();

    auto sharedJob = std::make_shared<SerializationPlan::Job>(std::move(job));
    auto errorText = std::make_shared<QString>();

    return QThreadFunctionTask<void>(
        [this, sharedJob, name, errorText](QThreadFunction<void>& task) {
            task.setAutoDelayedSync(false);

            task.setThreadFunctionData(
                [this, sharedJob, name, errorText]() mutable {
                    OperationContextScope scope(getOperationContext());

#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
                    printLine("Job started", name, 2);
#endif
                    try {
                        sharedJob->run();
                    }
                    catch (const std::exception& e) {
                        sharedJob->setException(e);
                    }
                    catch (...) {
                        sharedJob->setUnknownException();
                    }

#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
                    printLine("Job ended", name, 2);
#endif
                });
        },
        [this, name, sharedJob](const QThreadFunction<void>&, DoneWith doneWith) -> DoneResult {
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
            printLine("Task done", name, 2);
#endif

            if (doneWith == DoneWith::Cancel)
                return DoneResult::Error;

            if (sharedJob->hasError()) {
                auto result = resultAs<ProjectSaveResult>();

                result->_success        = false;
                result->_errorMessage   = sharedJob->getError();

	            return DoneResult::Error;
            }

            return DoneResult::Success;
        },
        CallDoneFlag::Always
    );
}

void SerializePlanWorkflow::setup(SerializePlanContext& context)
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Setup", 2);
#endif

	Application::setSerializationAborted(false);
}

void SerializePlanWorkflow::serialize(SerializePlanContext& context)
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Serialize project JSON", 2);
#endif
}

void SerializePlanWorkflow::finalize(SerializePlanContext& context)
{
#ifdef SERIALIZE_PLAN_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Finalize", 2);
#endif
}
