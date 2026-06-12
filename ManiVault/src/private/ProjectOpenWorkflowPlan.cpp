// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectOpenWorkflowPlan.h"
#include "ProjectOpenContext.h"
#include "ProjectManager.h"
#include "Archiver.h"
#include "Exception/SerializationException.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;
using namespace mv::workflow;

#ifdef _DEBUG
	//#define PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
#endif

UniqueWorkflowPlan createProjectOpenWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectOpenContext>(filePath);

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QStringLiteral("Open project"), context);

    plan->addSequentialStage("Setup", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

        auto state = jobExecutionContext->getState();

        if (!state)
            return;

        state->metrics().registerInteger(
            "project.data.bytes_loaded",
            "bytes",
            {
                { "displayName", "Total number of bytes loaded" }
            }
        );

        if (QFileInfo(context->getFilePath()).isDir())
	        throw std::runtime_error("Project file path may not be a directory");

		auto temporaryDirPath = context->getTemporaryDirectoryPath();

		if (!QFileInfo(temporaryDirPath).exists())
			throw std::runtime_error("Temporary directory does not exist");

		context->setWorkspaceJsonPath(QFileInfo(temporaryDirPath, "workspace.json").absoluteFilePath());
		context->setProjectJsonPath(QFileInfo(temporaryDirPath, "project.json").absoluteFilePath());
		context->setMetaJsonPath(QFileInfo(temporaryDirPath, "meta.json").absoluteFilePath());

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
		qDebug() << "Temp. Dir" << temporaryDirPath;
		qDebug() << "Workspace JSON" << context->getWorkspaceJsonPath();
		qDebug() << "Project JSON" << context->getProjectJsonPath();
		qDebug() << "Project meta JSON" << context->getMetaJsonPath();
#endif

		workspaces().reset();

		mv::projects().newProject();

		Application::requestOverrideCursor(Qt::WaitCursor);

		mv::projects().getCurrentProject()->setFilePath(context->getFilePath());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Extract project archive", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Extract project archive";
#endif

        Archiver archiver;

		archiver.extractSingleFile(context->getFilePath(), "project.json", context->getProjectJsonPath());
		archiver.extractSingleFile(context->getFilePath(), "meta.json", context->getMetaJsonPath());
		archiver.extractSingleFile(context->getFilePath(), "workspace.json", context->getWorkspaceJsonPath());


    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Read project JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Read project JSON";
#endif

        const auto filePath = context->getProjectJsonPath();

        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(filePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        if (data.isEmpty())
            throw std::runtime_error("No data read");

        auto jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        context->setProjectMap(jsonDocument.toVariant().toMap());

    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Open meta JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open meta JSON";
#endif

        if (!QFileInfo(context->getMetaJsonPath()).exists())
	        throw std::runtime_error("Meta JSON file does not exist");

		if (auto currentProject = mv::projects().getCurrentProject()) {
			currentProject->getProjectMetaAction().fromJsonFile(context->getMetaJsonPath());
		}
		else {
			throw std::runtime_error("No current project found");
		}
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1);

    plan->addNestedWorkflowStage("Open project JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& executionContext) mutable -> UniqueWorkflowPlan {
        return mv::projects().getCurrentProject()->fromVariantMapWorkflow(context->getProjectMap()["Project"].toMap());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 20.0);

    plan->addSequentialStage("Open workspace JSON", [context]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open workspace JSON";
#endif
        if (!QFileInfo(context->getWorkspaceJsonPath()).exists())
            throw std::runtime_error("Workspace JSON file does not exist");
    
        workspaces().loadWorkspace(context->getWorkspaceJsonPath());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addOnSuccessStage("Success", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On success";
#endif

        qDebug() << "Project opened successfully";
    });

    plan->addOnFailureStage("Failure", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On failure";
#endif

        qDebug() << "Failed to open project";
    });

    plan->addFinalizationStage("Finally", [context]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finally";
#endif

        qDebug() << "Cleaning up...";

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

        if (!context->getErrorMessage().isEmpty())
	        throw std::runtime_error(context->getErrorMessage().toStdString());

		mv::projects().getRecentProjectsAction().addRecentFilePath(context->getFilePath());
		mv::projects().setState(AbstractProjectManager::State::Idle);

		if (auto project = mv::projects().getCurrentProject()) {
			project->updateContributors();
		}
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    return plan;
}
