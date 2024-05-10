// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosModel.h"

#include "util/FileDownloader.h"

#ifdef _DEBUG
    //#define LEARNING_PAGE_VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;

QMap<LearningPageVideosModel::Column, LearningPageVideosModel::ColumHeaderInfo> LearningPageVideosModel::columnInfo = QMap<LearningPageVideosModel::Column, LearningPageVideosModel::ColumHeaderInfo>({
    { LearningPageVideosModel::Column::Title, { "Title" , "Title", "Video title" } },
    { LearningPageVideosModel::Column::Tags, { "Tags" , "Tags", "Video tags" } }
});

LearningPageVideosModel::LearningPageVideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    FileDownloader fileDownloader;

    connect(&fileDownloader, &FileDownloader::downloaded, this, [this, &fileDownloader]() -> void {
        const auto jsonDocument = QJsonDocument::fromJson(fileDownloader.downloadedData());
        const auto rootObject   = jsonDocument.object();

        for (const auto& key : rootObject.keys())
            appendRow(Row(rootObject[key].toObject()));
    });

    fileDownloader.download(QUrl("https://www.manivault.studio/api/learning-center.json"));
}

LearningPageVideosModel::Item::Item(QJsonObject jsonObject, bool editable /*= false*/) :
    QStandardItem()
{
}

const QJsonObject& LearningPageVideosModel::Item::getJsonObject() const
{
    return _jsonObject;
}

LearningPageVideosModel::TitleItem::TitleItem(QJsonObject jsonObject) :
    Item(jsonObject)
{
}

QVariant LearningPageVideosModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getJsonObject()["title"].toString();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

LearningPageVideosModel::TagsItem::TagsItem(QJsonObject jsonObject) :
    Item(jsonObject)
{
}

QVariant LearningPageVideosModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getJsonObject()["tags"].toString();

        case Qt::ToolTipRole:
            return "Tags: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}
