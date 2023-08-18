// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectMeta.h"
#include "Project.h"

#include "util/Serialization.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

ProjectMeta::ProjectMeta(Project* project, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("ProjectMeta"),
    _project(project),
    _applicationVersionAction(this, "Application Version"),
    _projectVersionAction(this, "Project Version"),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _splashScreenAction(this),
    _compressionAction(this)
{
}

ProjectMeta::ProjectMeta(const QString& filePath, QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("ProjectMeta"),
    _applicationVersionAction(this, "Application Version"),
    _projectVersionAction(this, "Project Version"),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _splashScreenAction(this),
    _compressionAction(this)
{
    try {
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile projectMetaJsonFile(filePath);

        if (!projectMetaJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray projectMetaByteArray = projectMetaJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(projectMetaByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()["ProjectMeta"].toMap());
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

void ProjectMeta::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _applicationVersionAction.fromParentVariantMap(variantMap);
    _projectVersionAction.fromParentVariantMap(variantMap);
    _titleAction.fromParentVariantMap(variantMap);
    _descriptionAction.fromParentVariantMap(variantMap);
    _tagsAction.fromParentVariantMap(variantMap);
    _commentsAction.fromParentVariantMap(variantMap);
    _contributorsAction.fromParentVariantMap(variantMap);
    _splashScreenAction.fromParentVariantMap(variantMap);
    _compressionAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectMeta::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _project->getApplicationVersionAction().insertIntoVariantMap(variantMap);
    _project->getProjectVersionAction().insertIntoVariantMap(variantMap);
    _project->getTitleAction().insertIntoVariantMap(variantMap);
    _project->getDescriptionAction().insertIntoVariantMap(variantMap);
    _project->getTagsAction().insertIntoVariantMap(variantMap);
    _project->getCommentsAction().insertIntoVariantMap(variantMap);
    _project->getContributorsAction().insertIntoVariantMap(variantMap);
    _project->getSplashScreenAction().insertIntoVariantMap(variantMap);
    _project->getCompressionAction().insertIntoVariantMap(variantMap);

    return variantMap;
}

}
