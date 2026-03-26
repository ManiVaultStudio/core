// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OpenProjectWorkflow.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

using namespace QtTaskTree;

OpenProjectWorkflow::OpenProjectWorkflow(mv::ProjectManager& projectManager, QObject* parent) :
	WorkflowBase<OpenProjectContext>(parent),
	_projectManager(projectManager),
    _loadTask(this, "Opening project..."),
    _setupTask(this, "Setting up project..."),
	_extractJsonTask(this, "Extracting project archive"),
    _loadDatasetsJsonTask(this, "Loading datasets from JSON"),
	_loadWorkspaceJsonTask(this, "Loading workspace from JSON")
{
    _setupTask.setParentTask(&_loadTask);
    _extractJsonTask.setParentTask(&_loadTask);
    _loadDatasetsJsonTask.setParentTask(&_loadTask);
    _loadWorkspaceJsonTask.setParentTask(&_loadTask);

    _setupTask.setWeight(.1f);
    _extractJsonTask.setWeight(.1f);
	_loadDatasetsJsonTask.setWeight(.7f);
	_loadWorkspaceJsonTask.setWeight(.1f);
}

void OpenProjectWorkflow::setInput(QString filePath, bool loadWorkspace, bool importDataOnly, bool disableReadOnly)
{
	_filePath        = std::move(filePath);
	_loadWorkspace   = loadWorkspace;
	_importDataOnly  = importDataOnly;
	_disableReadOnly = disableReadOnly;
}

void OpenProjectWorkflow::initializeContext(OpenProjectContext& storage)
{
	storage                 = {};
	storage.filePath        = _filePath;
	storage.loadWorkspace   = _loadWorkspace;
	storage.importDataOnly  = _importDataOnly;
	storage.disableReadOnly = _disableReadOnly;
}

void OpenProjectWorkflow::onStorageDone(const OpenProjectContext& storage)
{
	_finalError = storage.error;
}

void OpenProjectWorkflow::handleDone(QtTaskTree::DoneWith doneWith)
{
	const bool success = doneWith == QtTaskTree::DoneWith::Success && _finalError.isEmpty();
	emit finished(success, _finalError);
}

Group OpenProjectWorkflow::makeRecipe()
{
	auto& ctx = contextStorage();

	return Group{
		ctx,

		QSyncTask([this, &ctx] {
			auto& context = *ctx;

			try {
				setupOpenProject(context);
			}
			catch (const std::exception& e) {
				context.error = QString::fromUtf8(e.what());
			}
		}),
        QSyncTask([this, &ctx] {
            auto& context = *ctx;

            try {
                extractProjectArchive(context);
            }
            catch (const std::exception& e) {
                context.error = QString::fromUtf8(e.what());
            }
        }),
        _projectLoadRecipeBuilder.makeRecipe(_filePath, _projectLoadContextStorage),
        QSyncTask([this, &ctx] {
            auto& context = *ctx;

            try {
                finalizeOpenProject(context);
            }
            catch (const std::exception& e) {
                context.error = QString::fromUtf8(e.what());
            }
        })
	};
}

void OpenProjectWorkflow::setupOpenProject(OpenProjectContext& ctx)
{
    _loadTask.setRunning();

    _setupTask.setRunning();
    {
        if (QFileInfo(ctx.filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

        if (!temporaryDirectory.isValid())
            throw std::runtime_error("Unable to create temporary open-project directory");

        ctx.temporaryDirectoryPath  = temporaryDirectory.path();
        ctx.workspaceJsonPath       = QFileInfo(ctx.temporaryDirectoryPath, "workspace.json").absoluteFilePath();
        ctx.projectJsonPath         = QFileInfo(ctx.temporaryDirectoryPath, "project.json").absoluteFilePath();

        qDebug() << "Created temporary directory for opening project: " << ctx.temporaryDirectoryPath;
        qDebug() << "Workspace JSON path: " << ctx.workspaceJsonPath;
        qDebug() << "Project JSON path: " << ctx.projectJsonPath;

        Application::setSerializationAborted(false);

        workspaces().reset();

        if (!ctx.importDataOnly)
            _projectManager.newProject();

        Application::requestOverrideCursor(Qt::WaitCursor);

        _projectManager.getCurrentProject()->setFilePath(ctx.filePath);
    }
    _setupTask.setFinished();
}

void OpenProjectWorkflow::extractProjectArchive(OpenProjectContext& ctx)
{
    _extractJsonTask.setSubtasks(2);
    _extractJsonTask.setRunning();
	{
        Archiver archiver;

        archiver.extractSingleFile(ctx.filePath, "project.json", ctx.projectJsonPath);
        _extractJsonTask.setSubtaskFinished(0, "Datasets loaded");

		archiver.extractSingleFile(ctx.filePath, "workspace.json", ctx.workspaceJsonPath);
        _extractJsonTask.setSubtaskFinished(1, "Workspace loaded");
	}
    _extractJsonTask.setFinished();
}

void OpenProjectWorkflow::loadProjectJson(OpenProjectContext& ctx)
{
    _loadDatasetsJsonTask.setRunning();
    {
        projects().fromJsonFile(ctx.projectJsonPath);
    }
    _loadDatasetsJsonTask.setFinished();
}

void OpenProjectWorkflow::loadWorkspaceFromJson(OpenProjectContext& ctx)
{
    _loadWorkspaceJsonTask.setRunning();
	{
		const QFileInfo workspaceFileInfo(ctx.workspaceJsonPath);

		if (workspaceFileInfo.exists())
			workspaces().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);

		workspaces().setWorkspaceFilePath("");
	}
    _loadWorkspaceJsonTask.setFinished();
}

void OpenProjectWorkflow::finalizeOpenProject(OpenProjectContext& ctx)
{
    if (!ctx.error.isEmpty())
        throw std::runtime_error(ctx.error.toStdString());

    _projectManager.getRecentProjectsAction().addRecentFilePath(ctx.filePath);

    auto project = _projectManager.getCurrentProject();
    project->updateContributors();

    if (ctx.disableReadOnly)
        project->getReadOnlyAction().setChecked(false);

    //unsetTemporaryDirPath(TemporaryDirType::Open);
    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

    _loadTask.setFinished();
}
