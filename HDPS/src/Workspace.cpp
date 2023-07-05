// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Workspace.h"
#include "AbstractWorkspaceManager.h"

#include "Application.h"
#include "CoreInterface.h"

#include "util/Serialization.h"

#include <QBuffer>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

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

void Workspace::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    Serializable::fromVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::fromVariantMap(_titleAction, variantMap, "Title");
    Serializable::fromVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::fromVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::fromVariantMap(_commentsAction, variantMap, "Comments");

    workspaces().getLockingAction().setLocked(_lockingAction.isLocked());
}

QVariantMap Workspace::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    Serializable::insertIntoVariantMap(_lockingAction, variantMap, "Locking");
    Serializable::insertIntoVariantMap(_titleAction, variantMap, "Title");
    Serializable::insertIntoVariantMap(_descriptionAction, variantMap, "Description");
    Serializable::insertIntoVariantMap(_tagsAction, variantMap, "Tags");
    Serializable::insertIntoVariantMap(_commentsAction, variantMap, "Comments");

    return variantMap;
}

QImage Workspace::getPreviewImage(const QString& workspaceFilePath, const QSize& targetSize /*= QSize(500, 500)*/)
{
    QImage previewImage;

    try {
        if (!QFileInfo(workspaceFilePath).exists())
            throw std::runtime_error("File does not exist");

        QFile workspaceJsonFile(workspaceFilePath);

        if (!workspaceJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray workspaceByteArray = workspaceJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(workspaceByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        const auto workspaceVariantMap = jsonDocument.toVariant().toMap()["Workspace"].toMap();

        previewImage.loadFromData(QByteArray::fromBase64(workspaceVariantMap["PreviewImage"].toByteArray()));

        if (targetSize.isValid())
            return previewImage.scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        else
            return previewImage;
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to retrieve preview image from workspace file" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to retrieve preview image from workspace file";
    }

    return previewImage;
}

void Workspace::initialize()
{
    _titleAction.setPlaceHolderString("Enter workspace title here...");
    _titleAction.setClearable(true);

    _descriptionAction.setPlaceHolderString("Enter workspace description here...");
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");

    _commentsAction.setPlaceHolderString("Enter workspace comments here...");
    _commentsAction.setClearable(true);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);

    connect(&_lockingAction.getLockedAction(), &ToggleAction::toggled, this, [this]() -> void {
        for (auto plugin : plugins().getPluginsByType(plugin::Type::VIEW))
            dynamic_cast<hdps::plugin::ViewPlugin*>(plugin)->getLockingAction().setLocked(_lockingAction.isLocked());
    });

    connect(&workspaces().getLockingAction().getLockedAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        _lockingAction.setLocked(toggled);
    });
}

}
