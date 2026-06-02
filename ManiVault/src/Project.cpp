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

UniqueWorkflowPlan Project::fromVariantMapWorkflow(const QVariantMap& variantMap, SharedWorkflowExecutionContext parentExecutionContext)
{
    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1::fromVariantMap").arg(getSerializationName()));

    plan->addSequentialStage("Load", {
        WorkflowPlan::Job("Load", [this, variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& parentExecutionContext) {
            Serializable::fromVariantMap(variantMap);

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

    plan->addNestedWorkflowStage("Data hierarchy", [variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return mv::dataHierarchy().fromVariantMapWorkflow(variantMap, executionContext);
    });

    plan->addNestedWorkflowStage("Actions", [variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return mv::actions().fromVariantMapWorkflow(variantMap, executionContext);
    });

    plan->addNestedWorkflowStage("Plugins", [variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return mv::plugins().fromVariantMapWorkflow(variantMap, executionContext);
    });

    plan->addNestedWorkflowStage("Events", [variantMap](const WorkflowPlan::Job&, const SharedWorkflowExecutionContext& executionContext) -> UniqueWorkflowPlan {
        return mv::events().fromVariantMapWorkflow(variantMap, executionContext);
    });

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
    auto context = std::make_shared<ToVariantMapWorkflowContext>();

    UniqueWorkflowPlan plan = std::make_unique<WorkflowPlan>(QString("%1 (%2)").arg(__FUNCTION__).arg(getSerializationName()));

    plan->addSequentialStage("Common", [this, context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
        auto variantMap = Serializable::toVariantMap();

        _projectMetaAction.insertIntoVariantMap(variantMap);
        _selectionGroupingAction.insertIntoVariantMap(variantMap);
        _overrideApplicationStatusBarAction.insertIntoVariantMap(variantMap);
        _statusBarVisibleAction.insertIntoVariantMap(variantMap);
        _statusBarOptionsAction.insertIntoVariantMap(variantMap);

        context->setMap(variantMap);
    });

    WorkflowPlan::Jobs managerJobs;

    //managerJobs.emplace_back(plugins().getSerializationName(), [context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
    //    auto result = WorkflowRuntimeScoped::instance().executeBlocking(plugins().toVariantMapWorkflow(), executionContext);

    //    context->insertInto(plugins().getSerializationName(), result->value<QVariantMap>());
    //});

    //managerJobs.emplace_back(actions().getSerializationName(), [context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
    //    auto result = WorkflowRuntimeScoped::instance().executeBlocking(actions().toVariantMapWorkflow(), executionContext);

    //    context->insertInto(actions().getSerializationName(), result->value<QVariantMap>());
    //});

    //managerJobs.emplace_back(events().getSerializationName(), [context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
    //    auto result = WorkflowRuntimeScoped::instance().executeBlocking(events().toVariantMapWorkflow(), executionContext);

    //    context->insertInto(events().getSerializationName(), result->value<QVariantMap>());
    //});

    managerJobs.emplace_back(dataHierarchy().getSerializationName(), [context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
        auto result = WorkflowRuntimeScoped::instance().executeBlocking(dataHierarchy().toVariantMapWorkflow(), executionContext);

        context->insertInto(dataHierarchy().getSerializationName(), result->value<QVariantMap>()[dataHierarchy().getSerializationName()].toMap());
    });

    plan->addParallelStage("Managers", managerJobs);
    plan->addFinalizationStage("Set result", [this, context](const WorkflowPlan::Job&, [[maybe_unused]] const SharedWorkflowExecutionContext& executionContext) {
        executionContext->publishResultValue(getSerializationName(), context->getMap());
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
