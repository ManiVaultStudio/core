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
    ProjectOpenContext context;

    context._filePath = filePath;

    WorkflowPlan plan("Open project workflow", std::make_shared<ProjectOpenContext>(std::move(context)));

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Setup", 2);
#endif

    	auto projectOpenContext = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (QFileInfo(projectOpenContext->_filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

        if (!temporaryDirectory.isValid())
            throw std::runtime_error("Unable to create temporary open-project directory");

        projectOpenContext->_temporaryDirectoryPath = temporaryDirectory.path();
        projectOpenContext->_workspaceJsonPath      = QFileInfo(projectOpenContext->_temporaryDirectoryPath, "workspace.json").absoluteFilePath();
        projectOpenContext->_projectJsonPath        = QFileInfo(projectOpenContext->_temporaryDirectoryPath, "project.json").absoluteFilePath();

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Temp. Dir", projectOpenContext->_temporaryDirectoryPath, 3);
        printLine("Workspace JSON", projectOpenContext->_workspaceJsonPath, 3);
        printLine("Project JSON", projectOpenContext->_projectJsonPath, 3);
#endif

        Application::setSerializationAborted(false);

        workspaces().reset();

        mv::projects().newProject();

        Application::requestOverrideCursor(Qt::WaitCursor);

        mv::projects().getCurrentProject()->setFilePath(projectOpenContext->_filePath);
    });

    plan.addSequentialStage("Extract project archive", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
    printLine("Recipe stage", "Extract project archive", 2);
#endif

	    auto projectOpenContext = plan.getWorkflowContextAs<ProjectOpenContext>();

	    Archiver archiver;

	    archiver.extractSingleFile(projectOpenContext->_filePath, "project.json", projectOpenContext->_projectJsonPath);
		archiver.extractSingleFile(projectOpenContext->_filePath, "workspace.json", projectOpenContext->_workspaceJsonPath);
    });

    plan.addSequentialStage("Open project JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open project JSON", 2);
#endif

        auto projectOpenContext = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!QFileInfo(projectOpenContext->_projectJsonPath).exists())
            throw std::runtime_error("Project JSON file does not exist");
    
        if (auto currentProject = mv::projects().getCurrentProject()) {
            currentProject->fromJsonFile(projectOpenContext->_projectJsonPath);
        } else {
            throw std::runtime_error("No current project found");
        }
    });

    plan.addSequentialStage("Open workspace JSON", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Open workspace JSON", 2);
#endif

        auto projectOpenContext = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!QFileInfo(projectOpenContext->_workspaceJsonPath).exists())
            throw std::runtime_error("Workspace JSON file does not exist");
    
        workspaces().loadWorkspace(projectOpenContext->_workspaceJsonPath);
    });

    plan.addSequentialStage("Finalize", [&plan]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Finalize", 2);
#endif

        auto projectOpenContext = plan.getWorkflowContextAs<ProjectOpenContext>();

        if (!projectOpenContext->_errorMessage.isEmpty())
            throw std::runtime_error(projectOpenContext->_errorMessage.toStdString());
    
        mv::projects().getRecentProjectsAction().addRecentFilePath(projectOpenContext->_filePath);
    
        auto project = mv::projects().getCurrentProject();
    
        project->updateContributors();
    
        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    });
    
    return plan;
}
