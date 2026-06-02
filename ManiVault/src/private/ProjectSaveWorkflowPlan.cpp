// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSaveWorkflowPlan.h"
#include "ProjectSaveContext.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
	#define PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
#endif

#define PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE

UniqueWorkflowPlan createProjectSaveWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectSaveContext>(filePath);

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QStringLiteral("Save project"), context);

    plan->addSequentialStage("Setup", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

        Application::requestOverrideCursor(Qt::WaitCursor);

        if (QFileInfo(context->getFilePath()).isDir()) {
	        throw std::runtime_error("Project file path may not be a directory");
        }

		auto temporaryDirPath = context->getTemporaryDirectoryPath();

		if (!QFileInfo(temporaryDirPath).exists())
			throw std::runtime_error("Temporary directory does not exist");

		context->setWorkspaceJsonPath(QFileInfo(temporaryDirPath, "workspace.json").absoluteFilePath());
		context->setProjectJsonPath(QFileInfo(temporaryDirPath, "project.json").absoluteFilePath());
		context->setMetaJsonPath(QFileInfo(temporaryDirPath, "meta.json").absoluteFilePath());

#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
		qDebug() << "Workspace JSON" << context->getWorkspaceJsonPath();
		qDebug() << "Project JSON" << context->getProjectJsonPath();
		qDebug() << "Meta JSON" << context->getMetaJsonPath();
#endif
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    
    plan->addSequentialStage("Save project JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save project JSON";
#endif

        if (auto project = projects().getCurrentProject()) {
            project->toJsonFileScoped(context->getProjectJsonPath(), jobExecutionContext);
        }

    }, WorkflowPlan::JobThreadAffinity::GuiThread, 10.0);
    
    plan->addSequentialStage("Save meta JSON", [context]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save meta JSON";
#endif

        if (auto project = projects().getCurrentProject()) {
            project->getProjectMetaAction().toJsonFileScoped(context->getMetaJsonPath());
        }

    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 1.0);

    plan->addSequentialStage("Save workspace JSON", [&plan, context]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save workspace JSON";
#endif

        workspaces().saveWorkspace(context->getWorkspaceJsonPath(), false);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 2.0);

    plan->addSequentialStage("Archive", [&plan, context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Archive";
#endif

        QDir tempDir(context->getTemporaryDirectoryPath());

    	QStringList files = tempDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

        context->getArchiver().compressDirectory(context->getTemporaryDirectoryPath(), context->getFilePath(), true, 0);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Finalize", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finalize";
#endif

        if (!context->getErrorMessage().isEmpty())
            throw std::runtime_error(context->getErrorMessage().toStdString());

        mv::projects().getRecentProjectsAction().addRecentFilePath(context->getFilePath());

        if (auto project = projects().getCurrentProject()) {
            project->setFilePath(context->getFilePath());
            project->updateContributors();
        }
        else {
            throw std::runtime_error("Current project is null");
        }

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);
    
    return plan;
}
