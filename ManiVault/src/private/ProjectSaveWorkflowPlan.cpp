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

WorkflowPlan createProjectSaveWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectSaveContext>(filePath);

    WorkflowPlan plan("Save project workflow", context);

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Setup", 2);
#endif

        //OperationContextScope scope(getOperationContext());

        Application::requestOverrideCursor(Qt::WaitCursor);

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        if (QFileInfo(context->_filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        auto temporaryDirPath = context->_temporaryDirectory->path();

        if (!QFileInfo(temporaryDirPath).exists())
            throw std::runtime_error("Temporary directory does not exist");

        context->_workspaceJsonPath = QFileInfo(temporaryDirPath, "workspace.json").absoluteFilePath();
        context->_projectJsonPath   = QFileInfo(temporaryDirPath, "project.json").absoluteFilePath();
        context->_metaJsonPath      = QFileInfo(temporaryDirPath, "meta.json").absoluteFilePath();

#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
        printLine("Workspace JSON", context->_workspaceJsonPath, 3);
        printLine("Project JSON", context->_projectJsonPath, 3);
#endif

        //Application::setSerializationAborted(false);
    });

    plan.addSequentialStage("Save project JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Save project JSON", 2);
#endif

        //OperationContextScope scope(getOperationContext());

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        projects().toJsonFile(context->_projectJsonPath);
    });

    plan.addSequentialStage("Save project meta JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Save project meta JSON", 2);
#endif

        //OperationContextScope scope(getOperationContext());

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        if (auto project = projects().getCurrentProject()) {
            project->getProjectMetaAction().toJsonFile(context->_metaJsonPath);
        }
    });

    plan.addSequentialStage("Save workspace JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Finalize", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        workspaces().saveWorkspace(context->_workspaceJsonPath, false);
    });

    plan.addSequentialStage("Archive", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Archive", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        context->_archiver.compressDirectory(context->_temporaryDirectory->path(), context->_filePath, true, 0);
    });

    plan.addSequentialStage("Finalize", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        printLine("Recipe stage", "Finalize", 2);
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        if (!context->_errorMessage.isEmpty())
            throw std::runtime_error(context->_errorMessage.toStdString());

        mv::projects().getRecentProjectsAction().addRecentFilePath(context->_filePath);

        if (auto project = projects().getCurrentProject()) {
            project->setFilePath(context->_filePath);
            project->updateContributors();
        }
        else {
            throw std::runtime_error("Current project is null");
        }

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    });
    
    return plan;
}
