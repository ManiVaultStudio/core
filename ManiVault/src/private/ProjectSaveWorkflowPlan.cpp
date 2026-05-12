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

    WorkflowPlan plan(QStringLiteral("Save project"), context);

    plan.addSequentialStage("Setup", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

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

#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Workspace JSON" << context->_workspaceJsonPath;
        qDebug() << "Project JSON" << context->_projectJsonPath;
        qDebug() << "Meta JSON" << context->_metaJsonPath;
#endif
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Save project JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save project JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        projects().toJsonFile(context->_projectJsonPath);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 10.0);

    plan.addSequentialStage("Save meta JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save meta JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        if (auto project = projects().getCurrentProject()) {
            project->getProjectMetaAction().toJsonFile(context->_metaJsonPath);
        }
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 1.0);

    plan.addSequentialStage("Save workspace JSON", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Save workspace JSON";
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        workspaces().saveWorkspace(context->_workspaceJsonPath, false);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 2.0);

    plan.addSequentialStage("Archive", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Archive";
#endif

        auto context = plan.getWorkflowContextAs<ProjectSaveContext>();

        context->_archiver.compressDirectory(context->_temporaryDirectory->path(), context->_filePath, true, 0);
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan.addSequentialStage("Finalize", [&plan]() -> void {
#ifdef PROJECT_SAVE_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finalize";
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
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);
    
    return plan;
}
