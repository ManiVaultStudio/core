// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HelpManagerVideosModel.h"

#include <QJsonArray>
#include <QJsonObject>

#ifdef _DEBUG
    //#define HELP_MANAGER_VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;

QMap<HelpManagerVideosModel::Column, HelpManagerVideosModel::ColumHeaderInfo> HelpManagerVideosModel::columnInfo = QMap<HelpManagerVideosModel::Column, HelpManagerVideosModel::ColumHeaderInfo>({
    { HelpManagerVideosModel::Column::Title, { "Title" , "Title", "Video title" } },
    { HelpManagerVideosModel::Column::Tags, { "Tags" , "Tags", "Video tags" } },
    { HelpManagerVideosModel::Column::Date, { "Date" , "Date", "Video date" } },
    { HelpManagerVideosModel::Column::Summary, { "Summary" , "Summary", "Video description" } },
    { HelpManagerVideosModel::Column::YouTubeId, { "YouTube ID" , "YouTube ID", "YouTube video identifier" } },
    { HelpManagerVideosModel::Column::YouTubeUrl, { "YouTube URL" , "YouTube URL", "Video URL" } },
    { HelpManagerVideosModel::Column::Delegate, { "Delegate" , "Delegate", "Delegate" } }
});

HelpManagerVideosModel::HelpManagerVideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant HelpManagerVideosModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

QSet<QString> HelpManagerVideosModel::getTagsSet() const
{
    return _tags;
}

void HelpManagerVideosModel::populateFromServer()
{
    connect(&_fileDownloader, &FileDownloader::downloaded, this, [this]() -> void {
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

HelpManagerVideosModel::Item::Item(QVariantMap variantMap, bool editable /*= false*/) :
    QStandardItem(),
    _variantMap(variantMap)
{
}

const QVariantMap& HelpManagerVideosModel::Item::getVariantMap() const
{
    return _variantMap;
}

QVariant HelpManagerVideosModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant HelpManagerVideosModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant HelpManagerVideosModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant HelpManagerVideosModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant HelpManagerVideosModel::YouTubeIdItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant HelpManagerVideosModel::YouTubeUrlItem::data(int role /*= Qt::UserRole + 1*/) const
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
