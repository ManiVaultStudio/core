// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Project.h"
#include "AbstractProjectManager.h"
#include "CoreInterface.h"
#include "Set.h"

#include "util/Serialization.h"

#include <QTemporaryDir>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(),
    _startupProject(false),
    _applicationVersion(Application::current()->getVersion()),
    _projectMetaAction(this),    
    _task(this, "Project Task")
{
    initialize();
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

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(projectByteArray);

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
    _startupProject = filePath == Application::current()->getStartupProjectFilePath();

    emit filePathChanged(_filePath);
}

bool Project::isStartupProject() const
{
    return _startupProject;
}

void Project::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _projectMetaAction.getApplicationVersionAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getProjectVersionAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getReadOnlyAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getTitleAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getDescriptionAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getTagsAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getCommentsAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getContributorsAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getCompressionAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getSplashScreenAction().fromParentVariantMap(variantMap);
    _projectMetaAction.getStudioModeAction().fromParentVariantMap(variantMap);

    dataHierarchy().fromParentVariantMap(variantMap);
    actions().fromParentVariantMap(variantMap);
    plugins().fromParentVariantMap(variantMap);
}

QVariantMap Project::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();
    
    _projectMetaAction.getApplicationVersionAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getProjectVersionAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getReadOnlyAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getTitleAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getDescriptionAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getTagsAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getCommentsAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getContributorsAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getCompressionAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getSplashScreenAction().insertIntoVariantMap(variantMap);
    _projectMetaAction.getStudioModeAction().insertIntoVariantMap(variantMap);

    plugins().insertIntoVariantMap(variantMap);
    dataHierarchy().insertIntoVariantMap(variantMap);
    actions().insertIntoVariantMap(variantMap);

    return variantMap;
}

ModalTask& Project::getTask()
{
    return _task;
}

util::Version Project::getVersion() const
{
    return _applicationVersion;
}

void Project::updateContributors()
{
    QString currentUserName;

#ifdef __APPLE__
    currentUserName = getenv("USER");
#else
    currentUserName = getenv("USERNAME");
#endif

    if (!currentUserName.isEmpty() && !_projectMetaAction.getContributorsAction().getStrings().contains(currentUserName))
        _projectMetaAction.getContributorsAction().addString(currentUserName);
}

void Project::setStudioMode(bool studioMode)
{
    auto plugins = hdps::plugins().getPluginsByTypes();  // by default gets all plugin types
    auto& datasets = hdps::data().allSets();

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
    QTemporaryDir temporaryDir;

    const auto projectMetaJsonFilePath = projects().extractFileFromManiVaultProject(projectFilePath, temporaryDir, "meta.json");

    if (projectMetaJsonFilePath.isEmpty())
        return {};

    return QSharedPointer<ProjectMetaAction>::create(projectMetaJsonFilePath);
}

void Project::initialize()
{
    updateContributors();

    getStudioModeAction().setIcon(Application::getIconFont("FontAwesome").getIcon("pencil-ruler"));

    connect(&getStudioModeAction(), &ToggleAction::toggled, this, &Project::setStudioMode);

    const auto updateStudioModeActionReadOnly = [&]() -> void {
        getStudioModeAction().setEnabled(projects().hasProject());
    };

    updateStudioModeActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateStudioModeActionReadOnly);
}

}
