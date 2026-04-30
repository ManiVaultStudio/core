// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectOpenWorkflowPlan.h"
#include "ProjectOpenContext.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
	#define PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
#endif

WorkflowPlan createProjectOpenWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectOpenContext>(filePath);

    WorkflowPlan plan("Open project workflow", context);

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

        if (auto wokflowExecutionContext = WorkflowExecutionContext::current()) {
            auto state = wokflowExecutionContext->getState();

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

    	auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (QFileInfo(context->_filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        context->_temporaryDirectory.reset(new QTemporaryDir(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "SaveProject")));

        auto temporaryDirPath = context->_temporaryDirectory->path();

        if (!QFileInfo(temporaryDirPath).exists())
            throw std::runtime_error("Temporary directory does not exist");

        context->_temporaryDirectoryPath    = temporaryDirPath;
        context->_workspaceJsonPath         = QFileInfo(context->_temporaryDirectoryPath, "workspace.json").absoluteFilePath();
        context->_projectJsonPath           = QFileInfo(context->_temporaryDirectoryPath, "project.json").absoluteFilePath();
        context->_metaJsonPath       = QFileInfo(context->_temporaryDirectoryPath, "meta.json").absoluteFilePath();

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Temp. Dir" << context->_temporaryDirectoryPath;
        qDebug() << "Workspace JSON" << context->_workspaceJsonPath;
        qDebug() << "Project JSON" << context->_projectJsonPath;
        qDebug() << "Project meta JSON" << context->_metaJsonPath;
#endif

        workspaces().reset();

        mv::projects().newProject();

        Application::requestOverrideCursor(Qt::WaitCursor);

        mv::projects().getCurrentProject()->setFilePath(context->_filePath);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Extract project archive", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Extract project archive";
#endif

	    auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

	    Archiver archiver;

	    archiver.extractSingleFile(context->_filePath, "project.json", context->_projectJsonPath);
	    archiver.extractSingleFile(context->_filePath, "meta.json", context->_metaJsonPath);
		archiver.extractSingleFile(context->_filePath, "workspace.json", context->_workspaceJsonPath);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Open meta JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open meta JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!QFileInfo(context->_metaJsonPath).exists())
            throw std::runtime_error("Meta JSON file does not exist");

        if (auto currentProject = mv::projects().getCurrentProject()) {
            currentProject->getProjectMetaAction().fromJsonFile(context->_metaJsonPath);
        }
        else {
            throw std::runtime_error("No current project found");
        }
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1);

    plan.addSequentialStage("Open project JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open project JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!QFileInfo(context->_projectJsonPath).exists())
            throw std::runtime_error("Project JSON file does not exist");
    
        if (auto currentProject = mv::projects().getCurrentProject()) {
            currentProject->fromJsonFile(context->_projectJsonPath);
        } else {
            throw std::runtime_error("No current project found");
        }
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 20.0);

    plan.addSequentialStage("Open workspace JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open workspace JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!QFileInfo(context->_workspaceJsonPath).exists())
            throw std::runtime_error("Workspace JSON file does not exist");
    
        workspaces().loadWorkspace(context->_workspaceJsonPath);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Finalize", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finalize";
#endif

        auto context = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!context->_errorMessage.isEmpty())
            throw std::runtime_error(context->_errorMessage.toStdString());

        mv::projects().getRecentProjectsAction().addRecentFilePath(context->_filePath);
    
        auto project = mv::projects().getCurrentProject();
    
        project->updateContributors();

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);
    
    return plan;
}
