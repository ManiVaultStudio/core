// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Project.h"
#include "AbstractProjectManager.h"
#include "CoreInterface.h"
#include "Set.h"

#include "util/Serialization.h"

using namespace mv::gui;
using namespace mv::util;

namespace 
{
    /** Thread-safe context for constructing the variant map */
    class ToVariantMapWorkflowContext {
    public:

        /**
         * Thread-safe getter for the variant map being constructed
         * @return Variant map being constructed
         */
        QVariantMap getMap() const
        {
            QMutexLocker locker(&_mutex);
            return _map;
        }

        /**
         * Thread-safe inserter for the variant map being constructed
         * @param key Key to insert into the variant map
         * @param value Value to insert into the variant map
         */
        void insertInto(const QString& key, const QVariant& value)
        {
            QMutexLocker locker(&_mutex);
            _map.insert(key, value);
        }

        /**
         * Thread-safe setter for the variant map being constructed
         * @param map Variant map to set
         */
        void setMap(const QVariantMap& map)
        {
            QMutexLocker locker(&_mutex);
            _map = map;
        }

    private:
        mutable QMutex  _mutex;     /** Mutex for thread-safe access to the variant map */
        QVariantMap     _map;       /** Variant map being constructed */
    };
}

namespace mv {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _startupProject(false),
    _applicationVersion(Application::current()->getVersion()),
    _projectMetaAction(this),
    _selectionGroupingAction(this, "Selection grouping"),
    _overrideApplicationStatusBarAction(this, "Override studio status bar"),
    _statusBarVisibleAction(this, "Show status bar"),
    _statusBarOptionsAction(this, "Status bar items")
{
    initialize();

    /*
    const auto initStatusBarOptionsAction = [this]() -> void {
        const auto& appStatusBarOptionsAction = mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction();

        _statusBarOptionsAction.setOptions(mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction().getOptions());
    };

    initStatusBarOptionsAction();

    connect(&mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction(), &OptionsAction::optionsChanged, initStatusBarOptionsAction);
    */

    /*
    const auto updateStatusBarActions = [this]() -> void {
        const auto overrideApplicationStatusBar = _overrideApplicationStatusBarAction.isChecked();

        _statusBarVisibleAction.setEnabled(overrideApplicationStatusBar);
        _statusBarOptionsAction.setEnabled(overrideApplicationStatusBar);
    };

    updateStatusBarActions();

    connect(&_overrideApplicationStatusBarAction, &ToggleAction::toggled, updateStatusBarActions);
    */
}

Project::Project(const QString& filePath, QObject* parent /*= nullptr*/) :
    Project(parent)
{
    setFilePath(filePath);
    initialize();

    try {
        if (!QFileInfo(_filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile projectJsonFile(_filePath);

        if (!projectJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray projectByteArray = projectJsonFile.readAll();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(projectByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()["Project"].toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load project from file:" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load project from file";
    }
}

QString Project::getFilePath() const
{
    return _filePath;
}

void Project::setFilePath(const QString& filePath)
{
    _filePath       = filePath;
    _startupProject = filePath == Application::current()->getStartupProjectUrl();

    emit filePathChanged(_filePath);
}

bool Project::isStartupProject() const
{
    return _startupProject;
}

UniqueWorkflowPlan Project::fromVariantMapWorkflow(const QVariantMap& variantMap)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1::fromVariantMap").arg(getSerializationName()));

    plan->addNestedWorkflowStage("Load common", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return Serializable::fromVariantMapWorkflow(variantMap);
    });

    plan->addSequentialStage("Load", {
        WorkflowPlan::Job("Load", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
		    if (variantMap.contains(_selectionGroupingAction.getSerializationName())) {
		        _selectionGroupingAction.fromParentVariantMap(variantMap);
		    }
		    else {
		        _selectionGroupingAction.setChecked(true);
		    }

		    _overrideApplicationStatusBarAction.fromParentVariantMap(variantMap);
		    _statusBarVisibleAction.fromParentVariantMap(variantMap);
		    _statusBarOptionsAction.fromParentVariantMap(variantMap);
        })
    });

    WorkflowPlan::Jobs loadManagerJobs;

    const auto addManagerLoadStage = [&plan, &loadManagerJobs, variantMap](AbstractManager& manager) {
        const auto managerMap = variantMap[manager.getSerializationName()].toMap();

        plan->addNestedWorkflowStage(QString("Load %1").arg(manager.getSerializationName()), [managerMap, &manager](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
            return manager.fromVariantMapWorkflow(managerMap);
        }, WorkflowPlan::JobThreadAffinity::GuiThread);
    };

    addManagerLoadStage(mv::plugins());
    addManagerLoadStage(mv::actions());
    addManagerLoadStage(mv::dataHierarchy());
    addManagerLoadStage(mv::events());

    plan->addSequentialStage("Post-process", {
        WorkflowPlan::Job("Post-process", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& parentExecutionContext) {
	        if (getReadOnlyAction().isChecked() && getAllowedPluginsOnlyAction().isChecked()) {
				for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
					pluginFactory->setAllowPluginCreationFromStandardGui(_projectMetaAction.getAllowedPluginsAction().getStrings().contains(pluginFactory->getKind()));
			}
        })
    });

    return plan;
}

UniqueWorkflowPlan Project::toVariantMapWorkflow() const
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1 (%2)").arg(__FUNCTION__).arg(getSerializationName()));

    const auto saveCommonStage = plan->addSequentialStage("Save common", [this](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        auto outputMap = Serializable::toVariantMap();

        _projectMetaAction.insertIntoVariantMap(outputMap);
        _selectionGroupingAction.insertIntoVariantMap(outputMap);
        _overrideApplicationStatusBarAction.insertIntoVariantMap(outputMap);
        _statusBarVisibleAction.insertIntoVariantMap(outputMap);
        _statusBarOptionsAction.insertIntoVariantMap(outputMap);

        executionContext->setOutput(outputMap);
    });

    const auto savePluginsStage = plan->addNestedWorkflowStage(QString("Save %1").arg(mv::plugins().getSerializationName()), [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return mv::plugins().toVariantMapWorkflow();
	});

    const auto saveActionsStage = plan->addNestedWorkflowStage(QString("Save %1").arg(mv::actions().getSerializationName()), [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return mv::actions().toVariantMapWorkflow();
    });

    const auto saveDataHierarchyStage = plan->addNestedWorkflowStage(QString("Save %1").arg(mv::dataHierarchy().getSerializationName()), [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return mv::dataHierarchy().toVariantMapWorkflow();
    });

    const auto saveEventsStage = plan->addNestedWorkflowStage(QString("Save %1").arg(mv::events().getSerializationName()), [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return mv::events().toVariantMapWorkflow();
    });

    const auto saveWorkspacesStage = plan->addNestedWorkflowStage(QString("Save %1").arg(mv::workspaces().getSerializationName()), [](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext&) {
        return mv::workspaces().toVariantMapWorkflow();
    });

    plan->addSequentialStage("Publish result", [this, saveCommonStage, savePluginsStage, saveActionsStage, saveDataHierarchyStage, saveEventsStage, saveWorkspacesStage](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) {
        auto outputMap = executionContext->takeOutput(saveCommonStage).toMap();

        outputMap[mv::plugins().getSerializationName()]         = executionContext->takeOutput(savePluginsStage).toMap();
        outputMap[mv::actions().getSerializationName()]         = executionContext->takeOutput(saveActionsStage).toMap();
        outputMap[mv::dataHierarchy().getSerializationName()]   = executionContext->takeOutput(saveDataHierarchyStage).toMap();
        outputMap[mv::events().getSerializationName()]          = executionContext->takeOutput(saveEventsStage).toMap();
        outputMap[mv::workspaces().getSerializationName()]      = executionContext->takeOutput(saveWorkspacesStage).toMap();

    	executionContext->setOutput(outputMap);
    });

	return plan;
}

Version Project::getVersion() const
{
    return _applicationVersion;
}

void Project::updateContributors()
{
    QString currentUserName;

#ifdef __APPLE__
    currentUserName = qgetenv("USER");
#else
    currentUserName = qgetenv("USERNAME");
#endif

    if (!currentUserName.isEmpty() && !_projectMetaAction.getContributorsAction().getStrings().contains(currentUserName))
        _projectMetaAction.getContributorsAction().addString(currentUserName);
}

void Project::setStudioMode(bool studioMode)
{
    auto plugins    = mv::plugins().getPluginsByTypes();  // by default gets all plugin types
    auto datasets   = mv::data().getAllDatasets();

    if (studioMode) {
        for (auto plugin : plugins)
            plugin->cacheConnectionPermissions(true);

        for (auto plugin : plugins)
            plugin->setConnectionPermissionsToAll(true);

        for (auto& dataset : datasets)
        {
            for (auto& action : dataset.get()->getActions())
                action->cacheConnectionPermissions(true);

            for (auto& action : dataset.get()->getActions())
                action->setConnectionPermissionsToAll(true);
        }
    }
    else {
        for (auto plugin : plugins)
            plugin->restoreConnectionPermissions(true);

        for (auto& dataset : datasets)
            for (auto& action : dataset.get()->getActions())
                action->restoreConnectionPermissions(true);
    }

}

ProjectMetaAction& Project::getProjectMetaAction()
{
    return _projectMetaAction;
}

QSharedPointer<ProjectMetaAction> Project::getProjectMetaActionFromProjectFilePath(const QString& projectFilePath)
{
    const auto projectMetaJsonFilePath = projects().extractFileFromManiVaultProject(projectFilePath, Application::current()->getTemporaryDir(), "meta.json");

    if (projectMetaJsonFilePath.isEmpty())
        return {};

    return QSharedPointer<ProjectMetaAction>::create(projectMetaJsonFilePath);
}

void Project::initialize()
{
    Application::current()->getConfigurationAction().getBrandingConfigurationAction().getSplashScreenAction().setMayCloseSplashScreenWidget(true);

    updateContributors();

    getStudioModeAction().setIconByName("pen-ruler");

    connect(&getStudioModeAction(), &ToggleAction::toggled, this, &Project::setStudioMode);

    const auto updateStudioModeActionReadOnly = [&]() -> void {
        getStudioModeAction().setEnabled(projects().hasProject());
    };

    updateStudioModeActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateStudioModeActionReadOnly);

    _selectionGroupingAction.setIconByName("object-group");
    _selectionGroupingAction.setToolTip("Enable/disable dataset grouping");

    _statusBarOptionsAction.setDefaultWidgetFlag(OptionsAction::WidgetFlag::Selection);
}

}
