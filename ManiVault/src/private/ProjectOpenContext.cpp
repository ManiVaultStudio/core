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
	WorkflowBase<ProjectOpenContext>("Open Project", parent),
	_projectManager(projectManager),
    _setupTask(this, "Setting up project..."),
	_extractJsonTask(this, "Extracting project archive"),
    _loadDatasetsJsonTask(this, "Loading datasets from JSON"),
	_loadWorkspaceJsonTask(this, "Loading workspace from JSON")
{
    //_setupTask.setParentTask(&_loadTask);
    //_extractJsonTask.setParentTask(&_loadTask);
    //_loadDatasetsJsonTask.setParentTask(&_loadTask);
    //_loadWorkspaceJsonTask.setParentTask(&_loadTask);

 //   _setupTask.setWeight(.1f);
 //   _extractJsonTask.setWeight(.1f);
	//_loadDatasetsJsonTask.setWeight(.7f);
	//_loadWorkspaceJsonTask.setWeight(.1f);

    //QCoreApplication::processEvents();

    //auto task = new ModalTask(nullptr, "================= project...");

    //task->setRunning();
    //task->setEnabled(true);
    //task->setProgress(.8f, "//////////");
}

void OpenProjectWorkflow::setInput(QString filePath, bool loadWorkspace, bool importDataOnly, bool disableReadOnly)
{
	_filePath        = std::move(filePath);
	_loadWorkspace   = loadWorkspace;
	_importDataOnly  = importDataOnly;
	_disableReadOnly = disableReadOnly;
}

void OpenProjectWorkflow::initializeContext(ProjectOpenContext& storage)
{
	storage                 = {};
	storage.filePath        = _filePath;
	storage.loadWorkspace   = _loadWorkspace;
	storage.importDataOnly  = _importDataOnly;
	storage.disableReadOnly = _disableReadOnly;
}

Group OpenProjectWorkflow::makeRecipe()
{
    auto contextStorage = this->contextStorage();

	return Group{
        contextStorage,

		QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

			try {
                setup(*projectOpenContext);
			}
			catch (const std::exception& e) {
                projectOpenContext->error = QString::fromUtf8(e.what());
			}
		}),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                extractProjectArchive(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->error = QString::fromUtf8(e.what());
            }
        }),
        _projectLoadRecipeBuilder.makeRecipe(contextStorage, _projectLoadContextStorage),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                finalize(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->error = QString::fromUtf8(e.what());
            }
        })
	};
}

void OpenProjectWorkflow::setup(ProjectOpenContext& context)
{
    

    //_setupTask.setRunning();
    //{
        if (QFileInfo(context.filePath).isDir())
            throw std::runtime_error("Project file path may not be a directory");

        QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

        if (!temporaryDirectory.isValid())
            throw std::runtime_error("Unable to create temporary open-project directory");

        context.temporaryDirectoryPath  = temporaryDirectory.path();
        context.workspaceJsonPath       = QFileInfo(context.temporaryDirectoryPath, "workspace.json").absoluteFilePath();
        context.projectJsonPath         = QFileInfo(context.temporaryDirectoryPath, "project.json").absoluteFilePath();

        qDebug() << "Created temporary directory for opening project: " << context.temporaryDirectoryPath;
        qDebug() << "Workspace JSON path: " << context.workspaceJsonPath;
        qDebug() << "Project JSON path: " << context.projectJsonPath;

        Application::setSerializationAborted(false);

        workspaces().reset();

        if (!context.importDataOnly)
            _projectManager.newProject();

        Application::requestOverrideCursor(Qt::WaitCursor);

        _projectManager.getCurrentProject()->setFilePath(context.filePath);
    //}
    //_setupTask.setFinished();
}

void OpenProjectWorkflow::extractProjectArchive(ProjectOpenContext& context)
{
 //   _extractJsonTask.setSubtasks(2);
 //   _extractJsonTask.setRunning();
	//{
        Archiver archiver;

        archiver.extractSingleFile(context.filePath, "project.json", context.projectJsonPath);
        //_extractJsonTask.setSubtaskFinished(0, "Datasets loaded");

		archiver.extractSingleFile(context.filePath, "workspace.json", context.workspaceJsonPath);
        //_extractJsonTask.setSubtaskFinished(1, "Workspace loaded");
	//}
    //_extractJsonTask.setFinished();
}

void OpenProjectWorkflow::finalize(ProjectOpenContext& context)
{
    if (!context.error.isEmpty())
        throw std::runtime_error(context.error.toStdString());

    _projectManager.getRecentProjectsAction().addRecentFilePath(context.filePath);

    auto project = _projectManager.getCurrentProject();
    project->updateContributors();

    if (context.disableReadOnly)
        project->getReadOnlyAction().setChecked(false);

    //unsetTemporaryDirPath(TemporaryDirType::Open);
    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
}
