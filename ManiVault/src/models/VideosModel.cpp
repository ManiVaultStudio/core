// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "VideosModel.h"

#ifdef _DEBUG
    //#define VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;

namespace mv {

QMap<VideosModel::Column, VideosModel::ColumHeaderInfo> VideosModel::columnInfo = QMap<Column, ColumHeaderInfo>({
    { Column::Title, { "Title" , "Title", "Video title" } },
    { Column::Tags, { "Tags" , "Tags", "Video tags" } },
    { Column::Date, { "Date" , "Date", "Video date" } },
    { Column::Type, { "Type" , "Type", "Video type" } },
    { Column::Summary, { "Summary" , "Summary", "Video description" } },
    { Column::Resource, { "Resource" , "Resource", "Video resource" } },
    { Column::Delegate, { "Delegate" , "Delegate", "Delegate" } }
});

VideosModel::VideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

QVariant VideosModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
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

        case Column::Delegate:
            return DelegateItem::headerData(orientation, role);

        default:
            break;
    }

    return {};
}

QSet<QString> VideosModel::getTagsSet() const
{
    return _tags;
}

void VideosModel::addVideo(const Video* video)
{
    Q_ASSERT(video);

    if (!video)
        return;

    appendRow(Row(video));
    updateTags();

    const_cast<Video*>(video)->setParent(this);

    _videos.push_back(video);
}

void VideosModel::updateTags()
{
    for (int rowIndex = 0; rowIndex < rowCount(); ++rowIndex)
		for (const auto& tag : dynamic_cast<Item*>(itemFromIndex(index(rowIndex, 0)))->getVideo()->getTags())
			_tags.insert(tag);

    emit tagsChanged(_tags);
}

VideosModel::Item::Item(const mv::util::Video* video, bool editable /*= false*/) :
    _video(video)
{
}

const Video* VideosModel::Item::getVideo() const
{
    return _video;
}

QVariant VideosModel::TypeItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
	        return static_cast<int>(getVideo()->getType());

	    case Qt::DisplayRole: {
	        switch (getVideo()->getType()) {
		        case Video::Type::YouTube:
		            return "YouTube";

		        case Video::Type::GIF:
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

QVariant VideosModel::TitleItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant VideosModel::TagsItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant VideosModel::DateItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant VideosModel::SummaryItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant VideosModel::ResourceItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant VideosModel::ThumbnailItem::data(int role) const
{
    switch (role) {
	    case Qt::EditRole:
            return QVariant::fromValue(getVideo()->getThumbnailImage());

	    case Qt::DisplayRole:
        case Qt::ToolTipRole:
            break;

	    default:
	        break;
    }

    return Item::data(role);
}

}