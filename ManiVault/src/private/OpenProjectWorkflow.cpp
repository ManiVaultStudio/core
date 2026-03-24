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
	_projectManager(projectManager)
{
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
	storage                 = {}; // fine, this is value-initialization, not copy-assignment
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
	using namespace QtTaskTree;

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
        })
	};
}

void OpenProjectWorkflow::setupOpenProject(OpenProjectContext& ctx)
{
    qDebug() << __FUNCTION__ << ctx.filePath;

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

void OpenProjectWorkflow::extractProjectArchive(OpenProjectContext& ctx)
{
    Archiver archiver;

    archiver.extractSingleFile(ctx.filePath, "workspace.json", ctx.workspaceJsonPath);
    archiver.extractSingleFile(ctx.filePath, "project.json", ctx.projectJsonPath);
}

void OpenProjectWorkflow::loadProjectJson(OpenProjectContext& ctx)
{
    //projects().fromJsonFile(ctx.projectJsonPath);
}

void OpenProjectWorkflow::loadWorkspaceStage(OpenProjectContext& ctx)
{
    //const QFileInfo workspaceFileInfo(ctx.workspaceJsonPath);

    //if (workspaceFileInfo.exists())
    //    workspaces().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);

    //workspaces().setWorkspaceFilePath("");
}

void OpenProjectWorkflow::finalizeOpenProject(OpenProjectContext& ctx)
{
    //if (!ctx.error.isEmpty())
    //    throw std::runtime_error(ctx.error.toStdString());

    //_recentProjectsAction.addRecentFilePath(ctx.filePath);

    //_project->updateContributors();

    //if (ctx.disableReadOnly)
    //    _project->getReadOnlyAction().setChecked(false);

    //if (_project->isStartupProject())
    //    ModalTask::getGlobalHandler()->setEnabled(true);

    //if (_project->getOverrideApplicationStatusBarAction().isChecked()
    //    && _project->getStatusBarVisibleAction().isChecked()) {
    //    auto& miscellaneousSettings = mv::settings().getMiscellaneousSettings();
    //    miscellaneousSettings.getStatusBarVisibleAction()
    //        .setChecked(_project->getStatusBarVisibleAction().isChecked());
    //}

    //unsetTemporaryDirPath(TemporaryDirType::Open);
    //Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
    //setState(State::Idle);

    //emit projectOpened(*_project);
}
