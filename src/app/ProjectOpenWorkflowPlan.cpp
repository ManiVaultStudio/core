// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectOpenWorkflowPlan.h"
#include "ProjectOpenContext.h"
#include "ProjectManager.h"
#include "Archiver.h"

#include <CoreInterface.h>

#include <QFileInfo>
#include <QDir>
#include <QMetaType>
#include <QSet>

namespace
{
void collectPackedTileUris(const QVariant& value, QStringList& uris)
{
    if (value.metaType().id() == QMetaType::QVariantMap)
    {
        const auto map = value.toMap();
        if (map.value("Storage").toString() == QStringLiteral("PackedTileFiles"))
            for (const auto& superTileValue : map.value("SuperTiles").toList())
            {
                const auto uri = superTileValue.toMap().value("URI").toString().trimmed();
                if (!uri.isEmpty())
                    uris.push_back(uri);
            }

        for (auto it = map.cbegin(); it != map.cend(); ++it)
            collectPackedTileUris(it.value(), uris);
    }
    else if (value.metaType().id() == QMetaType::QVariantList)
    {
        for (const auto& item : value.toList())
            collectPackedTileUris(item, uris);
    }
}

bool isSafeArchiveEntryPath(const QString& path)
{
    if (path.isEmpty() || QDir::isAbsolutePath(path) || path.contains(QLatin1Char('\\')) || path.contains(QLatin1Char(':')))
        return false;

    const auto cleanPath = QDir::cleanPath(path);
    return cleanPath != QStringLiteral(".") && cleanPath != QStringLiteral("..") && !cleanPath.startsWith(QStringLiteral("../"));
}
}

using namespace mv;
using namespace mv::gui;
using namespace mv::util;
using namespace mv::workflow;

#ifdef _DEBUG
	//#define PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
#endif

UniqueWorkflowPlan createProjectOpenWorkflowPlan(const QString& filePath)
{
    auto context = std::make_shared<ProjectOpenContext>(filePath);

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("Open project %1").arg(QFileInfo(filePath).fileName()), context);

    plan->addSequentialStage("Setup", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Setup";
#endif

        auto state = jobExecutionContext->getState();

        if (!state)
            return;

        state->metrics().registerInteger(
            "project.data.bytes_loaded",
            "bytes",
            {
                { "displayName", "Total number of bytes loaded" }
            }
        );

        if (QFileInfo(context->getFilePath()).isDir())
	        throw std::runtime_error("Project file path may not be a directory");

		auto temporaryDirPath = context->getTemporaryDirectoryPath();

		if (!QFileInfo(temporaryDirPath).exists())
			throw std::runtime_error("Temporary directory does not exist");

		context->setWorkspaceJsonPath(QFileInfo(temporaryDirPath, "workspace.json").absoluteFilePath());
		context->setProjectJsonPath(QFileInfo(temporaryDirPath, "project.json").absoluteFilePath());
		context->setMetaJsonPath(QFileInfo(temporaryDirPath, "meta.json").absoluteFilePath());

#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
		qDebug() << "Temp. Dir" << temporaryDirPath;
		qDebug() << "Workspace JSON" << context->getWorkspaceJsonPath();
		qDebug() << "Project JSON" << context->getProjectJsonPath();
		qDebug() << "Project meta JSON" << context->getMetaJsonPath();
#endif

		workspaces().reset();

		mv::projects().newProject();
		static_cast<ProjectManager&>(mv::projects()).prepareProjectStagingDirectory();

		Application::requestOverrideCursor(Qt::WaitCursor);

		mv::projects().getCurrentProject()->setFilePath(context->getFilePath());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Extract project archive", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Extract project archive";
#endif

        Archiver archiver;

        archiver.extractSingleFile(context->getFilePath(), "meta.json", context->getMetaJsonPath());
		archiver.extractSingleFile(context->getFilePath(), "project.json", context->getProjectJsonPath());
		archiver.extractSingleFile(context->getFilePath(), "workspace.json", context->getWorkspaceJsonPath());

    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    plan->addSequentialStage("Open meta JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext&) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Open meta JSON";
#endif

        if (!QFileInfo(context->getMetaJsonPath()).exists())
            throw std::runtime_error("Meta JSON file does not exist");

        if (auto currentProject = mv::projects().getCurrentProject()) {
            currentProject->getProjectMetaAction().fromJsonFile(context->getMetaJsonPath());
        }
        else {
            throw std::runtime_error("No current project found");
        }
        }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    WorkflowPlan::Jobs readJsonJobs;

    readJsonJobs.emplace_back("Read project JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Read project JSON";
#endif

        const auto filePath = context->getProjectJsonPath();

        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(filePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        if (data.isEmpty())
            throw std::runtime_error("No data read");

        auto jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        context->setProjectMap(jsonDocument.toVariant().toMap());

    });

    readJsonJobs.emplace_back("Read workspace JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext& jobExecutionContext) -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Read workspace JSON";
#endif

        const auto filePath = context->getWorkspaceJsonPath();

        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(filePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        if (data.isEmpty())
            throw std::runtime_error("No data read");

        auto jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        context->setWorkspaceMap(jsonDocument.toVariant().toMap());
    });

    plan->addParallelStage("Read JSON files", std::move(readJsonJobs));

    plan->addSequentialStage("Stage packed point-data files", [context](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        QStringList uris;
        collectPackedTileUris(context->getProjectMap(), uris);

        QStringList archiveEntries;
        QSet<QString> extracted;
        for (const auto& uri : uris)
        {
            if (!isSafeArchiveEntryPath(uri) || extracted.contains(uri))
                continue;

            extracted.insert(uri);
            archiveEntries.push_back(uri);
        }

        executionContext->setProgress(0.0);
        const auto stagingDirectory = mv::projects().getTemporaryDirPath(AbstractProjectManager::TemporaryDirType::Staging);
        Archiver archiver;
        for (qsizetype index = 0; index < archiveEntries.size(); ++index)
        {
            const auto& uri = archiveEntries[index];

            const auto destinationPath = QDir(stagingDirectory).filePath(QDir::cleanPath(uri));
            if (!QDir().mkpath(QFileInfo(destinationPath).absolutePath()))
                throw std::runtime_error(QString("Unable to create staging directory for packed point-data file: %1").arg(uri).toStdString());

            archiver.extractSingleFile(context->getFilePath(), uri, destinationPath);
            if (!QFileInfo(destinationPath).isFile())
                throw std::runtime_error(QString("Packed point-data file is missing from the project archive: %1").arg(uri).toStdString());

            executionContext->setProgress(static_cast<double>(index + 1) / static_cast<double>(archiveEntries.size()));
        }
    }, WorkflowPlan::JobThreadAffinity::CurrentWorkerThread, 10.0);

    plan->addNestedWorkflowStage("Open project JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext&) mutable -> UniqueWorkflowPlan {
        return mv::projects().getCurrentProject()->fromVariantMapWorkflow(context->getProjectMap()["Project"].toMap());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 100.0);

    plan->addNestedWorkflowStage("Open workspace JSON", [context](const WorkflowPlan::Job& job, const SharedWorkflowExecutionContext&) mutable -> UniqueWorkflowPlan {
        return workspaces().fromVariantMapWorkflow(context->getWorkspaceMap()["Workspace"].toMap());
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 10.0);

    plan->addOnSuccessStage("Success", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On success";
#endif

        qDebug() << "Project opened successfully";


    });

    plan->addOnFailureStage("Failure", []() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "On failure";
#endif

        qDebug() << "Failed to open project";
    });

    plan->addFinalizationStage("Finally", [context]() -> void {
#ifdef PROJECT_OPEN_WORKFLOW_PLAN_VERBOSE
        qDebug() << "Finally";
#endif

        qDebug() << "Cleaning up...";

        Application::requestRemoveOverrideCursor(Qt::WaitCursor, true);

        if (!context->getErrorMessage().isEmpty())
	        throw std::runtime_error(context->getErrorMessage().toStdString());

		mv::projects().getRecentProjectsAction().addRecentFilePath(context->getFilePath());
		mv::projects().setState(AbstractProjectManager::State::Idle);

		if (auto project = mv::projects().getCurrentProject()) {
			project->updateContributors();

            const auto appVersion = mv::projects().getCurrentProject()->getApplicationVersionAction().getVersion();

            if (appVersion < Version(1, 5, 0)) {
                const auto messageTitle = QStringLiteral("Project can be optimized");
                const auto messageText = QStringLiteral("<p>This project was saved with an earlier version of ManiVault Studio (before 1.5.0). It is fully supported, but does not benefit from the new high-performance project storage architecture.</p>"
                    "<p>Save the project to convert it to the new format and take advantage of substantially faster loading and improved storage efficiency.</p>");

                mv::help().addNotification(messageTitle, messageText, StyledIcon("info"));

            	qDebug() << messageText;
            }
		}
    }, WorkflowPlan::JobThreadAffinity::GuiThread, 1.0);

    return plan;
}
