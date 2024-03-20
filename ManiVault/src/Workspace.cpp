// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Workspace.h"
#include "AbstractWorkspaceManager.h"

#include "Application.h"
#include "CoreInterface.h"

#include "util/Serialization.h"

#include <QBuffer>

using namespace mv::gui;
using namespace mv::util;

namespace mv {

Workspace::Workspace(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(),
    _lockingAction(this),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();
}

Workspace::Workspace(const QString& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Workspace"),
    _filePath(filePath),
    _lockingAction(this),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments")
{
    initialize();

    try {
        if (!QFileInfo(getFilePath()).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(getFilePath());

        if (!workspaceJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray workspaceByteArray = workspaceJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(workspaceByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()["Workspace"].toMap());
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load workspace from file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load workspace from file";
    }
}

QString Workspace::getFilePath() const
{
    return _filePath;
}

void Workspace::setFilePath(const QString& filePath)
{
    _filePath = filePath;

    emit filePathChanged(_filePath);
}

void Workspace::fromVariantMap(const mv::VariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::fromVariantMap(_titleAction, variantMap, "Title");
    Serializable::fromVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::fromVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::fromVariantMap(_commentsAction, variantMap, "Comments");

    workspaces().getLockingAction().setLocked(_lockingAction.isLocked());
}

mv::VariantMap Workspace::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    Serializable::insertIntoVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::insertIntoVariantMap(_titleAction, variantMap, "Title");
    Serializable::insertIntoVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::insertIntoVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::insertIntoVariantMap(_commentsAction, variantMap, "Comments");

    return variantMap;
}

void Workspace::initialize()
{
    _titleAction.setPlaceHolderString("Enter workspace title here...");
    _titleAction.setClearable(true);

    _descriptionAction.setPlaceHolderString("Enter workspace description here...");
    _descriptionAction.setClearable(true);

    _tagsAction.setIconByName("tag");
    _tagsAction.setCategory("Tag");

    _commentsAction.setPlaceHolderString("Enter workspace comments here...");
    _commentsAction.setClearable(true);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);

    connect(&_lockingAction.getLockedAction(), &ToggleAction::toggled, this, [this]() -> void {
        for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW))
            dynamic_cast<mv::plugin::ViewPlugin*>(plugin)->getLockingAction().setLocked(_lockingAction.isLocked());
    });

    connect(&workspaces().getLockingAction().getLockedAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        _lockingAction.setLocked(toggled);
    });
}

}
