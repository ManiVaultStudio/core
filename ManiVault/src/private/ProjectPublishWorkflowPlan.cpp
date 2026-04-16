// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectPublishWorkflowPlan.h"
#include "ProjectPublishContext.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
	#define PROJECT_PUBLISH_WORKFLOW_PLAN_VERBOSE
#endif

WorkflowPlan createProjectPublishWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectPublishContext>(filePath);

    WorkflowPlan plan("Publish project workflow", context);

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_PUBLISH_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Setup", 2);
#endif

    	auto context = plan.getWorkflowContextAs<ProjectPublishContext>();

        if (QFileInfo(context->_filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        context->_temporaryDirectory.reset(new QTemporaryDir(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "SaveProject")));

        auto temporaryDirPath = context->_temporaryDirectory->path();

        if (!QFileInfo(temporaryDirPath).exists())
            throw std::runtime_error("Temporary directory does not exist");

        context->_temporaryDirectoryPath = temporaryDirPath;
        context->_workspaceJsonPath      = QFileInfo(context->_temporaryDirectoryPath, "workspace.json").absoluteFilePath();
        context->_projectJsonPath        = QFileInfo(context->_temporaryDirectoryPath, "project.json").absoluteFilePath();

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Temp. Dir", context->_temporaryDirectoryPath, 3);
        printLine("Workspace JSON", context->_workspaceJsonPath, 3);
        printLine("Project JSON", context->_projectJsonPath, 3);
#endif

        workspaces().reset();

        mv::projects().newProject();

        Application::requestOverrideCursor(Qt::WaitCursor);

        mv::projects().getCurrentProject()->setFilePath(context->_filePath);
    });

    plan.addSequentialStage("Extract project archive", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
    printLine("Recipe stage", "Extract project archive", 2);
#endif

	    auto context = plan.getWorkflowContextAs<ProjectPublishContext>();

	    Archiver archiver;

	    archiver.extractSingleFile(context->_filePath, "project.json", context->_projectJsonPath);
		archiver.extractSingleFile(context->_filePath, "workspace.json", context->_workspaceJsonPath);
    });

    plan.addSequentialStage("Open project JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open project JSON", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectPublishContext>();

        if (!QFileInfo(context->_projectJsonPath).exists())
            throw std::runtime_error("Project JSON file does not exist");
    
        if (auto currentProject = mv::projects().getCurrentProject()) {
            currentProject->fromJsonFile(context->_projectJsonPath);
        } else {
            throw std::runtime_error("No current project found");
        }
    });

    plan.addSequentialStage("Open workspace JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open workspace JSON", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectPublishContext>();

        if (!QFileInfo(context->_workspaceJsonPath).exists())
            throw std::runtime_error("Workspace JSON file does not exist");
    
        workspaces().loadWorkspace(context->_workspaceJsonPath);
    });

    plan.addSequentialStage("Finalize", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Finalize", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectPublishContext>();

        if (!context->_errorMessage.isEmpty())
            throw std::runtime_error(context->_errorMessage.toStdString());
    
        mv::projects().getRecentProjectsAction().addRecentFilePath(context->_filePath);
    
        auto project = mv::projects().getCurrentProject();
    
        project->updateContributors();
    
        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    });
    
    return plan;
}
