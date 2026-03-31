// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectOpenWorkflow.h"
#include "ProjectManager.h"
#include "Archiver.h"
#include "ProjectOpenResult.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

using namespace QtTaskTree;

#ifdef _DEBUG
	#define PROJECT_OPEN_WORKFLOW_VERBOSE
#endif

ProjectOpenWorkflow::ProjectOpenWorkflow(const QString& filePath, QObject* parent) :
	AbstractWorkflow(createContext(filePath), "Open Project", parent)
{
}

Group ProjectOpenWorkflow::makeRecipe()
{
#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Workflow", "Make recipe", 1);
#endif

    auto& contextStorage = this->contextStorage();

	return Group{
        contextStorage,

		QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

			try {
                setup(*projectOpenContext);
			}
			catch (const std::exception& e) {
                projectOpenContext->_errorMessage = QString::fromUtf8(e.what());
			}
		}),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                extractProjectArchive(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->_errorMessage = QString::fromUtf8(e.what());
            }
        }),
        //_projectLoadRecipeBuilder.makeRecipe(contextStorage, _projectLoadContextStorage),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                finalize(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->_errorMessage = QString::fromUtf8(e.what());
            }
        })
	};
}

void ProjectOpenWorkflow::setupStorage(WorkflowRuntimeContext& context)
{
#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Workflow", "Setup storage", 1);
#endif
}

void ProjectOpenWorkflow::onStorageDone(const WorkflowRuntimeContext& context)
{
#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Workflow", "Storage done", 1);
#endif
}

void ProjectOpenWorkflow::handleDone(QtTaskTree::DoneWith status)
{
    AbstractWorkflow::handleDone(status);

#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Workflow", "Handle done", 1);
#endif

    if (const auto result = resultAs<ProjectOpenResult>()) {
        const auto duration = getDuration();
        const auto text     = (duration < 1000) ? QString("%1 opened successfully in %2 ms").arg(result->_filePath).arg(duration) : QString("%1 saved successfully in %2 s").arg(result->_filePath).arg(duration / 1000.0, 0, 'f', 1);

        if (status == QtTaskTree::DoneWith::Success)
            help().addNotification("Project opened", text, StyledIcon("folder-open"));
        else
            help().addNotification("Error", "Unable to open ManiVault project: " + result->_errorMessage, StyledIcon("exclamation-triangle"));
    } else {
        throw std::runtime_error("Unexpected error: ProjectOpenResult is null");
    }

    emit finished(status == DoneWith::Success, QString{});
}

void ProjectOpenWorkflow::initResult(UniqueWorkflowResultBase& result)
{
    result.reset(new ProjectOpenResult());
}

UniqueWorkflowContext ProjectOpenWorkflow::createContext(const QString& filePath)
{
    auto context = std::make_unique<ProjectOpenContext>();

    context->_filePath = filePath;

    return context;
}

void ProjectOpenWorkflow::setup(ProjectOpenContext& context)
{
    #ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
        printLine("Recipe stage", "Setup", 2);
    #endif

    if (QFileInfo(context._filePath).isDir())
        throw std::runtime_error("Project file path may not be a directory");

    QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

    if (!temporaryDirectory.isValid())
        throw std::runtime_error("Unable to create temporary open-project directory");

    context._temporaryDirectoryPath  = temporaryDirectory.path();
    context._workspaceJsonPath       = QFileInfo(context._temporaryDirectoryPath, "workspace.json").absoluteFilePath();
    context._projectJsonPath         = QFileInfo(context._temporaryDirectoryPath, "project.json").absoluteFilePath();

#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Temp. Dir", context._temporaryDirectoryPath, 3);
    printLine("Workspace JSON", context._workspaceJsonPath, 3);
    printLine("Project JSON", context._projectJsonPath, 3);
#endif

    Application::setSerializationAborted(false);

    workspaces().reset();

	mv::projects().newProject();

    Application::requestOverrideCursor(Qt::WaitCursor);

    mv::projects().getCurrentProject()->setFilePath(context._filePath);
}

void ProjectOpenWorkflow::extractProjectArchive(ProjectOpenContext& context)
{
#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Extract project archive", 2);
#endif
		
    Archiver archiver;

    archiver.extractSingleFile(context._filePath, "project.json", context._projectJsonPath);
	archiver.extractSingleFile(context._filePath, "workspace.json", context._workspaceJsonPath);
}

void ProjectOpenWorkflow::finalize(ProjectOpenContext& context)
{
#ifdef PROJECT_OPEN_WORKFLOW_VERBOSE
    printLine("Recipe stage", "Finalize", 2);
#endif

    if (!context._errorMessage.isEmpty())
        throw std::runtime_error(context._errorMessage.toStdString());

    mv::projects().getRecentProjectsAction().addRecentFilePath(context._filePath);

    auto project = mv::projects().getCurrentProject();

    project->updateContributors();

    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
}
