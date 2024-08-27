// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosModel.h"

#include <QJsonArray>
#include <QJsonDocument>

#ifdef _DEBUG
    //#define LEARNING_PAGE_VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;

QMap<LearningPageVideosModel::Column, LearningPageVideosModel::ColumHeaderInfo> LearningPageVideosModel::columnInfo = QMap<LearningPageVideosModel::Column, LearningPageVideosModel::ColumHeaderInfo>({
    { LearningPageVideosModel::Column::Title, { "Title" , "Title", "Video title" } },
    { LearningPageVideosModel::Column::Tags, { "Tags" , "Tags", "Video tags" } },
    { LearningPageVideosModel::Column::Date, { "Date" , "Date", "Video date" } },
    { LearningPageVideosModel::Column::Summary, { "Summary" , "Summary", "Video description" } },
    { LearningPageVideosModel::Column::YouTubeId, { "YouTube ID" , "YouTube ID", "YouTube video identifier" } },
    { LearningPageVideosModel::Column::YouTubeUrl, { "YouTube URL" , "YouTube URL", "Video URL" } },
    { LearningPageVideosModel::Column::Delegate, { "Delegate" , "Delegate", "Delegate" } }
});

LearningPageVideosModel::LearningPageVideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant LearningPageVideosModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::Tags:
            return TagsItem::headerData(orientation, role);

        case Column::Date:
            return DateItem::headerData(orientation, role);

        case Column::Summary:
            return SummaryItem::headerData(orientation, role);

        case Column::YouTubeId:
            return YouTubeIdItem::headerData(orientation, role);

        case Column::YouTubeUrl:
            return YouTubeUrlItem::headerData(orientation, role);

        case Column::Delegate:
            return DelegateItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

QSet<QString> LearningPageVideosModel::getTagsSet() const
{
    return _tags;
}

void LearningPageVideosModel::populateFromServer()
{
    connect(&_fileDownloader, &mv::util::FileDownloader::downloaded, this, [this]() -> void {
        setRowCount(0);

        const auto jsonDocument = QJsonDocument::fromJson(_fileDownloader.downloadedData());
        const auto videos       = jsonDocument.object()["videos"].toArray();

        for (const auto& video : videos) {
            const auto videoMap = video.toVariant().toMap();

            appendRow(Row(videoMap));

            for (const auto& tagVariant : videoMap["tags"].toList())
                _tags.insert(tagVariant.toString());
        }

        emit tagsChanged(_tags);
    });

    _fileDownloader.download(QUrl("https://www.manivault.studio/api/learning-center.json"));
}

LearningPageVideosModel::Item::Item(QVariantMap variantMap, bool editable /*= false*/) :
    QStandardItem(),
    _variantMap(variantMap)
{
}

const QVariantMap& LearningPageVideosModel::Item::getVariantMap() const
{
    return _variantMap;
}

QVariant LearningPageVideosModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVariantMap()["title"].toString();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningPageVideosModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getVariantMap()["tags"].toStringList();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toStringList().join(",");

        case Qt::ToolTipRole:
            return "Tags: " + data(Qt::DisplayRole).toStringList().join(",");

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningPageVideosModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVariantMap()["date"].toString().chopped(15);

        case Qt::ToolTipRole:
            return "Date: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningPageVideosModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVariantMap()["summary"].toString();

        case Qt::ToolTipRole:
            return "Summary: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningPageVideosModel::YouTubeIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVariantMap()["youtube-id"].toString();

        case Qt::ToolTipRole:
            return "YouTube identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningPageVideosModel::YouTubeUrlItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVariantMap()["url"].toString();

        case Qt::ToolTipRole:
            return "YouTube URL: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}
