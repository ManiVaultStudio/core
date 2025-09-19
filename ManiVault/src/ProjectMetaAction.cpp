// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectMetaAction.h"

#include "CoreInterface.h"
#include "Application.h"
#include "Project.h"

#include "util/Serialization.h"

using namespace mv::gui;
using namespace mv::util;

namespace mv {

ProjectMetaAction::ProjectMetaAction(Project* project, QObject* parent /*= nullptr*/) :
    VerticalGroupAction(parent, "ProjectMeta"),
    _project(project),
    _applicationVersionAction(this, "Application Version"),
    _projectVersionAction(this, "Project Version"),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _splashScreenAction(this, true),
    _studioModeAction(this, "Studio Mode"),
    _applicationIconAction(this, "Application icon"),
    _compressionAction(this),
    _allowProjectSwitchingAction(this, "Allow project switching"),
    _allowedPluginsOnlyAction(this, "Limit use of plugins"),
    _allowedPluginsAction(this, "Allowed Plugins")
{
    _splashScreenAction.setProjectMetaAction(this);

    _readOnlyAction.setToolTip("Whether the project is in read-only mode or not");

    _titleAction.setPlaceHolderString("Enter project title here...");
    _titleAction.setClearable(true);

    _descriptionAction.setPlaceHolderString("Enter project description here...");
    _descriptionAction.setClearable(true);

    _tagsAction.setIconByName("tag");
    _tagsAction.setCategory("Tag");
    _tagsAction.setStretch(2);

    _commentsAction.setPlaceHolderString("Enter project comments here...");
    _commentsAction.setClearable(true);
    _commentsAction.setStretch(2);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);

    _contributorsAction.setIconByName("user");
    _contributorsAction.setCategory("Contributor");
    _contributorsAction.setEnabled(false);
    _contributorsAction.setStretch(1);
    _contributorsAction.setDefaultWidgetFlags(StringsAction::ListView);

    _applicationIconAction.setToolTip("Application icon settings");

    _allowProjectSwitchingAction.setToolTip("Allow switching to another database project from the main menu");
    _allowedPluginsOnlyAction.setToolTip("Only plugins in the allowed list may be created from the UI.");
    _allowedPluginsAction.setToolTip("Only plugins in this list may be created from the UI.");

    _allowedPluginsAction.setCategory("Plugin name");

    _allowedPluginsCompleter.setCompletionMode(QCompleter::PopupCompletion);
    _allowedPluginsCompleter.setCaseSensitivity(Qt::CaseInsensitive);
    _allowedPluginsCompleter.setModel(&_allowedPluginsModel);

    _allowedPluginsAction.getNameAction().setCompleter(&_allowedPluginsCompleter);

    const auto updateAllowedPluginsModel = [this]() -> void {
        const auto loadedPlugins = mv::plugins().getLoadedPluginKinds();
        const auto lockedPlugins = _allowedPluginsAction.getLockedStrings();

        _allowedPluginsModel.setStringList(QSet<QString>(loadedPlugins.begin(), loadedPlugins.end()).subtract(QSet<QString>(lockedPlugins.begin(), lockedPlugins.end())).values());
    };

    updateAllowedPluginsModel();

    connect(&_allowedPluginsAction, &StringsAction::lockedStringsChanged, this, updateAllowedPluginsModel);

    const auto updateAllowedPluginsAction = [this]() -> void {
        _allowedPluginsAction.setEnabled(_allowedPluginsOnlyAction.isChecked());
    };

    updateAllowedPluginsAction();

    connect(&_allowedPluginsOnlyAction, &ToggleAction::toggled, this, updateAllowedPluginsAction);
}

ProjectMetaAction::ProjectMetaAction(const QString& filePath, QObject* parent /*= nullptr*/) :
    ProjectMetaAction(nullptr, parent)
{
    try {
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile ProjectMetaActionJsonFile(filePath);

        if (!ProjectMetaActionJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray ProjectMetaActionByteArray = ProjectMetaActionJsonFile.readAll();

        const auto jsonDocument = QJsonDocument::fromJson(ProjectMetaActionByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()[text()].toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load project meta data from file:" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load project meta data from file";
    }
}

void ProjectMetaAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _applicationVersionAction.fromParentVariantMap(variantMap);
    _projectVersionAction.fromParentVariantMap(variantMap);
    _readOnlyAction.fromParentVariantMap(variantMap);
    _titleAction.fromParentVariantMap(variantMap);
    _descriptionAction.fromParentVariantMap(variantMap);
    _tagsAction.fromParentVariantMap(variantMap);
    _commentsAction.fromParentVariantMap(variantMap);
    _contributorsAction.fromParentVariantMap(variantMap);
    _splashScreenAction.fromParentVariantMap(variantMap);
    _studioModeAction.fromParentVariantMap(variantMap);
    _applicationIconAction.fromParentVariantMap(variantMap);
    _compressionAction.fromParentVariantMap(variantMap);
    _allowProjectSwitchingAction.fromParentVariantMap(variantMap, true);
    _allowedPluginsOnlyAction.fromParentVariantMap(variantMap, true);
    _allowedPluginsAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ProjectMetaAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _applicationVersionAction.insertIntoVariantMap(variantMap);
    _projectVersionAction.insertIntoVariantMap(variantMap);
    _readOnlyAction.insertIntoVariantMap(variantMap);
    _titleAction.insertIntoVariantMap(variantMap);
    _descriptionAction.insertIntoVariantMap(variantMap);
    _tagsAction.insertIntoVariantMap(variantMap);
    _commentsAction.insertIntoVariantMap(variantMap);
    _contributorsAction.insertIntoVariantMap(variantMap);
    _splashScreenAction.insertIntoVariantMap(variantMap);
    _studioModeAction.insertIntoVariantMap(variantMap);
    _applicationIconAction.insertIntoVariantMap(variantMap);
    _compressionAction.insertIntoVariantMap(variantMap);
    _allowProjectSwitchingAction.insertIntoVariantMap(variantMap);
    _allowedPluginsOnlyAction.insertIntoVariantMap(variantMap);
    _allowedPluginsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

Project* ProjectMetaAction::getProject() const
{
    return _project;
}

}
