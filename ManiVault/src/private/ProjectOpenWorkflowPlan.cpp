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

#ifdef _DEBUG
	#define PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
#endif

WorkflowPlan createProjectOpenWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectOpenContext>(filePath);

    WorkflowPlan plan(QStringLiteral("Open project"), context);

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

        if (auto workflowExecutionContext = WorkflowExecutionContext::current()) {
            auto state = workflowExecutionContext->getState();

            if (!state)
                return;

            state->metrics().registerInteger(
                "project.data.bytes_loaded",
                "bytes",
                {
                    { "displayName", "Total number of bytes loaded" }
                }
            );
        }

        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {
	        if (QFileInfo(context->getFilePath()).isDir())
	        	throw std::runtime_error("Project file path may not be a directory");

			auto temporaryDirPath = context->getTemporaryDirectory()->path();

			if (!QFileInfo(temporaryDirPath).exists())
				throw std::runtime_error("Temporary directory does not exist");

			context->setWorkspaceJsonPath(QFileInfo(context->getTemporaryDirectoryPath(), "workspace.json").absoluteFilePath());
			context->setProjectJsonPath(QFileInfo(context->getTemporaryDirectoryPath(), "project.json").absoluteFilePath());
			context->setMetaJsonPath(QFileInfo(context->getTemporaryDirectoryPath(), "meta.json").absoluteFilePath());

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
			qDebug() << "Temp. Dir" << context->getTemporaryDirectoryPath();
			qDebug() << "Workspace JSON" << context->getWorkspaceJsonPath();
			qDebug() << "Project JSON" << context->getProjectJsonPath();
			qDebug() << "Project meta JSON" << context->getMetaJsonPath();
#endif

			workspaces().reset();

			mv::projects().newProject();

			Application::requestOverrideCursor(Qt::WaitCursor);

			mv::projects().getCurrentProject()->setFilePath(context->getFilePath());
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Extract project archive", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Extract project archive";
#endif

        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {
	        Archiver archiver;

			archiver.extractSingleFile(context->getFilePath(), "project.json", context->getProjectJsonPath());
			archiver.extractSingleFile(context->getFilePath(), "meta.json", context->getMetaJsonPath());
			archiver.extractSingleFile(context->getFilePath(), "workspace.json", context->getWorkspaceJsonPath());
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Open meta JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open meta JSON";
#endif

        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {
	        if (!QFileInfo(context->getMetaJsonPath()).exists())
	        	throw std::runtime_error("Meta JSON file does not exist");

			if (auto currentProject = mv::projects().getCurrentProject()) {
				currentProject->getProjectMetaAction().fromJsonFile(context->getMetaJsonPath());
			}
			else {
				throw std::runtime_error("No current project found");
			}
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1);

    plan.addSequentialStage("Open project JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
		qDebug() << "Open project JSON";
#endif

        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {
	        if (!QFileInfo(context->getProjectJsonPath()).exists())
	        	throw std::runtime_error("Project JSON file does not exist");

			if (auto currentProject = mv::projects().getCurrentProject()) {
				currentProject->fromJsonFile(context->getProjectJsonPath());
			}
			else {
				throw std::runtime_error("No current project found");
			}
        }
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 20.0);

    plan.addSequentialStage("Open workspace JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open workspace JSON";
#endif
        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {

            if (!QFileInfo(context->getWorkspaceJsonPath()).exists())
                throw std::runtime_error("Workspace JSON file does not exist");
        
            workspaces().loadWorkspace(context->getWorkspaceJsonPath());
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addOnSuccessStage("Success", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On success";
#endif

        qDebug() << "Project opened successfully";
    });

    plan.addOnFailureStage("Failure", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On failure";
#endif

        qDebug() << "Failed to open project";
    });

    plan.addFinalizationStage("Finally", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finally";
#endif

        qDebug() << "Cleaning up...";

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

        if (auto context = plan.getWorkflowContextAs<ProjectOpenContext>()) {
	        if (!context->getErrorMessage().isEmpty())
	        	throw std::runtime_error(context->getErrorMessage().toStdString());

			mv::projects().getRecentProjectsAction().addRecentFilePath(context->getFilePath());
			mv::projects().setState(AbstractProjectManager::State::Idle);

			auto project = mv::projects().getCurrentProject();

			project->updateContributors();
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    return plan;
}
