// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterVideosModel.h"

#ifdef _DEBUG
    //#define LEARNING_CENTER_VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

QMap<LearningCenterVideosModel::Column, LearningCenterVideosModel::ColumHeaderInfo> LearningCenterVideosModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Video title" } },
    { Column::Tags, { "Tags" , "Tags", "Video tags" } },
    { Column::Date, { "Date" , "Date", "Video date" } },
    { Column::Type, { "Type" , "Type", "Video type" } },
    { Column::Summary, { "Summary" , "Summary", "Video description" } },
    { Column::Resource, { "Resource" , "Resource", "Video resource" } },
    { Column::Thumbnail, { "Thumbnail" , "Thumbnail", "Thumbnail" } },
    { Column::Delegate, { "Delegate" , "Delegate", "Delegate" } }
});

LearningCenterVideosModel::LearningCenterVideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant LearningCenterVideosModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    switch (static_cast<Column>(section))
    {
        case Column::Title:
            return TitleItem::headerData(orientation, role);

        case Column::Tags:
            return TagsItem::headerData(orientation, role);

        case Column::Date:
            return DateItem::headerData(orientation, role);

        case Column::Type:
            return TypeItem::headerData(orientation, role);

        case Column::Summary:
            return SummaryItem::headerData(orientation, role);

        case Column::Resource:
            return ResourceItem::headerData(orientation, role);

        case Column::Thumbnail:
            return ThumbnailItem::headerData(orientation, role);

        case Column::Delegate:
            return DelegateItem::headerData(orientation, role);

        case Column::Count:
        default:
            break;
    }

    return {};
}

QSet<QString> LearningCenterVideosModel::getTagsSet() const
{
    return _tags;
}

void LearningCenterVideosModel::addVideo(const LearningCenterVideo* video)
{
    Q_ASSERT(video);

    if (!video)
        return;

    appendRow(Row(video));
    updateTags();

    const_cast<LearningCenterVideo*>(video)->setParent(this);

    _videos.push_back(video);
}

void LearningCenterVideosModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
        for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getVideo()->getTags())
            _tags.insert(tag);

    emit tagsChanged(_tags);
}

LearningCenterVideosModel::Item::Item(const mv::util::LearningCenterVideo* video, bool editable /*= false*/) :
    _video(video)
{
}

const LearningCenterVideo* LearningCenterVideosModel::Item::getVideo() const
{
    return _video;
}

QVariant LearningCenterVideosModel::TypeItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return static_cast<int>(getVideo()->getType());

	    case Qt::DisplayRole: {
	        switch (getVideo()->getType()) {
		        case LearningCenterVideo::Type::YouTube:
		            return "YouTube";

		        case LearningCenterVideo::Type::GIF:
		            return "GIF";
	        }

	        break;
    }

    case Qt::ToolTipRole:
        return "Type: " + data(Qt::DisplayRole).toString();

    default:
        break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVideo()->getTitle();

        case Qt::ToolTipRole:
            return "Title: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getVideo()->getTags();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toStringList().join(",");

        case Qt::ToolTipRole:
            return "Tags: " + data(Qt::DisplayRole).toStringList().join(",");

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVideo()->getDate();

        case Qt::ToolTipRole:
            return "Date: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVideo()->getSummary();

        case Qt::ToolTipRole:
            return "Summary: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::ResourceItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getVideo()->getResource();

        case Qt::ToolTipRole:
            return "Resource: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant LearningCenterVideosModel::ThumbnailItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return QVariant::fromValue(getVideo()->getThumbnailImage());

	    case Qt::DisplayRole:
        case Qt::ToolTipRole:
	    default:
	        break;
    }

    return Item::data(role);
}

}