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

ProjectOpenWorkflow::ProjectOpenWorkflow(const QString& filePath, QObject* parent) :
	AbstractWorkflow(createContext(filePath), "Open Project", parent)
{
}

Group ProjectOpenWorkflow::makeRecipe()
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
                projectOpenContext->_error = QString::fromUtf8(e.what());
			}
		}),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                extractProjectArchive(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->_error = QString::fromUtf8(e.what());
            }
        }),
        //_projectLoadRecipeBuilder.makeRecipe(contextStorage, _projectLoadContextStorage),
        QSyncTask([&] {
            auto projectOpenContext = contextAs<ProjectOpenContext>(contextStorage);

            try {
                finalize(*projectOpenContext);
            }
            catch (const std::exception& e) {
                projectOpenContext->_error = QString::fromUtf8(e.what());
            }
        })
	};
}

void ProjectOpenWorkflow::setupStorage(WorkflowRuntimeContext& context)
{
    qDebug() << __FUNCTION__;
}

void ProjectOpenWorkflow::onStorageDone(const WorkflowRuntimeContext& context)
{
    qDebug() << __FUNCTION__;
}

void ProjectOpenWorkflow::initResult(UniqueWorkflowResultBase& result)
{
    result = UniqueProjectOpenResult();
}

UniqueWorkflowContext ProjectOpenWorkflow::createContext(const QString& filePath)
{
    auto context = std::make_unique<ProjectOpenContext>();

    context->_filePath = filePath;

    return context;
}

void ProjectOpenWorkflow::setup(ProjectOpenContext& context)
{
    if (QFileInfo(context._filePath).isDir())
        throw std::runtime_error("Project file path may not be a directory");

    QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

    if (!temporaryDirectory.isValid())
        throw std::runtime_error("Unable to create temporary open-project directory");

    context._temporaryDirectoryPath  = temporaryDirectory.path();
    context._workspaceJsonPath       = QFileInfo(context._temporaryDirectoryPath, "workspace.json").absoluteFilePath();
    context._projectJsonPath         = QFileInfo(context._temporaryDirectoryPath, "project.json").absoluteFilePath();

    qDebug() << "Created temporary directory for opening project: " << context._temporaryDirectoryPath;
    qDebug() << "Workspace JSON path: " << context._workspaceJsonPath;
    qDebug() << "Project JSON path: " << context._projectJsonPath;
    Application::setSerializationAborted(false);

    workspaces().reset();

	mv::projects().newProject();

    Application::requestOverrideCursor(Qt::WaitCursor);

    mv::projects().getCurrentProject()->setFilePath(context._filePath);
}

void ProjectOpenWorkflow::extractProjectArchive(ProjectOpenContext& context)
{
    Archiver archiver;

    archiver.extractSingleFile(context._filePath, "project.json", context._projectJsonPath);
	archiver.extractSingleFile(context._filePath, "workspace.json", context._workspaceJsonPath);
}

void ProjectOpenWorkflow::finalize(ProjectOpenContext& context)
{
    if (!context._error.isEmpty())
        throw std::runtime_error(context._error.toStdString());

    mv::projects().getRecentProjectsAction().addRecentFilePath(context._filePath);

    auto project = mv::projects().getCurrentProject();

    project->updateContributors();

    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
}
