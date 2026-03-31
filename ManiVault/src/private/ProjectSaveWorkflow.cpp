// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectSaveWorkflow.h"
#include "ProjectManager.h"
#include "Archiver.h"

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

using namespace QtTaskTree;

ProjectSaveWorkflow::ProjectSaveWorkflow(mv::ProjectManager& projectManager, const QString& filePath, QObject* parent) :
	WorkflowBase<ProjectSaveContext>("Project Save", parent),
	_projectManager(projectManager)
{
    _projectSaveContext._filePath = filePath;
}

void ProjectSaveWorkflow::initializeContext(ProjectSaveContext& projectSaveContext)
{
    projectSaveContext._filePath = _projectSaveContext._filePath;
}

Group ProjectSaveWorkflow::makeRecipe()
{
    auto contextStorage = this->contextStorage();

	return Group{
		contextStorage,

		QSyncTask([&] {
			auto& context = *contextStorage;

			try {
                setup(context);
			}
			catch (const std::exception& e) {
				context._error = QString::fromUtf8(e.what());
			}
		}),
        QSyncTask([&] {
            auto& context = *contextStorage;

            try {
                save(context);
            }
            catch (const std::exception& e) {
                context._error = QString::fromUtf8(e.what());
            }
        }),
        QSyncTask([&] {
            auto& context = *contextStorage;

            try {
                finalize(context);
            }
            catch (const std::exception& e) {
                context._error = QString::fromUtf8(e.what());
            }
        })
	};
}

void ProjectSaveWorkflow::setup(ProjectSaveContext& context)
{
    qDebug() << __FUNCTION__;

    if (QFileInfo(context._filePath).isDir())
        throw std::runtime_error("Project file path may not be a directory");

    QTemporaryDir temporaryDirectory(QDir::cleanPath(Application::current()->getTemporaryDir().path() + QDir::separator() + "OpenProject"));

    if (!temporaryDirectory.isValid())
        throw std::runtime_error("Unable to create temporary save-project directory");

    context._temporaryDirectoryPath  = temporaryDirectory.path();
    context._workspaceJsonPath       = QFileInfo(context._temporaryDirectoryPath, "workspace.json").absoluteFilePath();
    context._projectJsonPath         = QFileInfo(context._temporaryDirectoryPath, "project.json").absoluteFilePath();

    qDebug() << "Created temporary directory for saving project: " << context._temporaryDirectoryPath;

	Application::setSerializationAborted(false);
    Application::requestOverrideCursor(Qt::WaitCursor);
}

void ProjectSaveWorkflow::save(ProjectSaveContext& context)
{
    qDebug() << __FUNCTION__;
}

void ProjectSaveWorkflow::finalize(ProjectSaveContext& context)
{
    qDebug() << __FUNCTION__;

    if (!context._error.isEmpty())
        throw std::runtime_error(context._error.toStdString());

    _projectManager.getRecentProjectsAction().addRecentFilePath(context._filePath);

    //auto project = _projectManager.getCurrentProject();
    //project->updateContributors();

    Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);
}
