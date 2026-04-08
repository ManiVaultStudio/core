// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSaveWorkflow.h"
#include "ProjectSaveResult.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

using namespace QtTaskTree;

#ifdef _DEBUG
	#define PROJECT_SAVE_WORKFLOW_VERBOSE
#endif

ProjectSaveWorkflow::ProjectSaveWorkflow(const QString& filePath, QObject* parent) :
    AbstractWorkflow(createContext(filePath), "Project Save", parent)
{
}

Group ProjectSaveWorkflow::makeRecipe()
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Workflow", "Make recipe", 1);
#endif

    auto& contextStorage = this->contextStorage();

	return Group{
		contextStorage,

		QSyncTask([&] {
			auto projectSaveContext = contextAs<ProjectSaveContext>(contextStorage);

			try {
                setup(*projectSaveContext);
                return true;
			}
			catch (const std::exception& e) {
				projectSaveContext->_errorMessage = QString::fromUtf8(e.what());
                return false;
			}
		}),
        QSyncTask([&] {
            auto projectSaveContext = contextAs<ProjectSaveContext>(contextStorage);

            try {
                saveProjectJson(*projectSaveContext);
                return true;
            }
            catch (const std::exception& e) {
                projectSaveContext->_errorMessage = QString::fromUtf8(e.what());
                return false;
            }
        }),
        QSyncTask([&] {
            auto projectSaveContext = contextAs<ProjectSaveContext>(contextStorage);

            try {
                saveProjectMetaJson(*projectSaveContext);
                return true;
            }
            catch (const std::exception& e) {
                projectSaveContext->_errorMessage = QString::fromUtf8(e.what());
                return false;
            }
        }),
        QSyncTask([&] {
            auto projectSaveContext = contextAs<ProjectSaveContext>(contextStorage);

            try {
                saveWorkspaceJson(*projectSaveContext);
                return true;
            }
            catch (const std::exception& e) {
                projectSaveContext->_errorMessage = QString::fromUtf8(e.what());
                return false;
            }
        }),
        QSyncTask([&] {
            auto projectSaveContext = contextAs<ProjectSaveContext>(contextStorage);

            try {
                finalize(*projectSaveContext);
                return true;
            }
            catch (const std::exception& e) {
                projectSaveContext->_errorMessage = QString::fromUtf8(e.what());
                return false;
            }
        })
	};
}

void ProjectSaveWorkflow::setupStorage(WorkflowRuntimeContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Workflow", "Setup storage", 1);
#endif

    OperationContextScope scope(getOperationContext());
}

void ProjectSaveWorkflow::onStorageDone(const WorkflowRuntimeContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Workflow", "Storage done", 1);
#endif
}

void ProjectSaveWorkflow::handleDone(QtTaskTree::DoneWith status)
{
    AbstractWorkflow::handleDone(status);
    
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Workflow", "Handle done", 1);
#endif

    if (auto currentProject = mv::projects().getCurrentProject()) {
	    if (const auto result = resultAs<ProjectSaveResult>()) {
	        const auto duration     = getDuration();
	        const auto successText  = (duration < 1000) ? QString("%1 saved successfully in %2 ms").arg(currentProject->getFilePath()).arg(duration) : QString("%1 saved successfully in %2 s").arg(result->_filePath).arg(duration / 1000.0, 0, 'f', 1);
	        const auto errorText    = "Unable to save ManiVault project: " + result->_errorMessage;

	        if (status == QtTaskTree::DoneWith::Success) {
	            help().addNotification("Project saved", successText, StyledIcon("floppy-disk"));
	            qDebug() << successText;
	        } else {
	            help().addNotification("Error", errorText, StyledIcon("exclamation-triangle"));
	            qWarning() << errorText;
	        }
	    } else {
	        throw std::runtime_error("Unexpected error: ProjectSaveResult is null");
	    }

	    if (getOperationContext()->hasErrors()) {
	        help().addNotification("Project saved with errors", getOperationContext()->combinedErrorMessage(), StyledIcon("exclamation-triangle"));
            qDebug() << "Project save completed with errors:" << getOperationContext()->combinedErrorMessage();
	    }
    }

    emit finished(status == DoneWith::Success, QString{});
}

void ProjectSaveWorkflow::initResult(UniqueWorkflowResultBase& result)
{
    result.reset(new ProjectSaveResult());
}

UniqueWorkflowContext ProjectSaveWorkflow::createContext(const QString& filePath)
{
    auto context = std::make_unique<ProjectSaveContext>();

    context->_filePath = filePath;

    return context;
}

void ProjectSaveWorkflow::setup(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Setup", 2);
#endif

    OperationContextScope scope(getOperationContext());

    Application::requestOverrideCursor(Qt::WaitCursor);

    if (QFileInfo(context._filePath).isDir())
        throw std::runtime_error("Project file path may not be a directory");

    context._temporaryDirectory = UniqueTemporaryDir(new QTemporaryDir(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject")));

    if (!QFileInfo(context._temporaryDirectory->path()).exists())
        throw std::runtime_error("Unable to create temporary save-project directory");

    context._temporaryDirectoryPath  = context._temporaryDirectory->path();
    context._workspaceJsonPath       = QFileInfo(context._temporaryDirectoryPath, "workspace.json").absoluteFilePath();
    context._projectJsonPath         = QFileInfo(context._temporaryDirectoryPath, "project.json").absoluteFilePath();

#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Temp. Dir", context._temporaryDirectoryPath, 3);
    printLine("Workspace JSON", context._workspaceJsonPath, 3);
    printLine("Project JSON", context._projectJsonPath, 3);
#endif

	Application::setSerializationAborted(false);
}

void ProjectSaveWorkflow::saveProjectJson(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Save project JSON", 2);
#endif

    OperationContextScope scope(getOperationContext());

    projects().toJsonFile(context._projectJsonPath);
}

void ProjectSaveWorkflow::saveProjectMetaJson(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Save project meta JSON", 2);
#endif

    OperationContextScope scope(getOperationContext());

    if (auto project = projects().getCurrentProject()) {
        //project->getProjectMetaAction().toJsonFile(projectMetaJsonFileInfo.absoluteFilePath());
    }
}

void ProjectSaveWorkflow::saveWorkspaceJson(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Save workspace JSON", 2);
#endif

    workspaces().saveWorkspace(context._workspaceJsonPath, false);
}

void ProjectSaveWorkflow::archive(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Archive", 2);
#endif

    context._archiver.compressDirectory(context._temporaryDirectoryPath, context._filePath, true, 0);
}

void ProjectSaveWorkflow::finalize(ProjectSaveContext& context)
{
#ifdef PROJECT_SAVE_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Finalize", 2);
#endif

    if (!context._errorMessage.isEmpty())
        throw std::runtime_error(context._errorMessage.toStdString());

    mv::projects().getRecentProjectsAction().addRecentFilePath(context._filePath);

    if (auto project = projects().getCurrentProject()) {
        project->setFilePath(context._filePath);
        project->updateContributors();
    } else {
        throw std::runtime_error("Current project is null");
    }

    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
}
