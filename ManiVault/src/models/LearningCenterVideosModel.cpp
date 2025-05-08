// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterVideosModel.h"

#include <QtConcurrent>

#ifdef _DEBUG
    //#define LEARNING_CENTER_VIDEOS_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

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
    QStandardItemModel(parent),
    _dsnsAction(this, "Data Source Names")
{
    setColumnCount(static_cast<int>(Column::Count));

    _dsnsAction.setIconByName("globe");
    _dsnsAction.setToolTip("Videos Data Source Names (DSN)");
    _dsnsAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _dsnsAction.setDefaultWidgetFlags(StringsAction::WidgetFlag::ListView);
    _dsnsAction.setPopupSizeHint(QSize(550, 100));

    connect(&_dsnsAction, &StringsAction::stringsChanged, this, [this]() -> void {
        setRowCount(0);

        _future = QtConcurrent::mapped(
            _dsnsAction.getStrings(),
            [this](const QString& dsn) {
                return downloadVideosFromDsn(dsn);
            });

        connect(&_watcher, &QFutureWatcher<QByteArray>::finished, [&]() {
            for (int dsnIndex = 0; dsnIndex < _dsnsAction.getStrings().size(); ++dsnIndex) {
                QJsonParseError jsonParseError;

                const auto jsonDocument = QJsonDocument::fromJson(_future.resultAt<QByteArray>(dsnIndex), &jsonParseError);

                if (jsonParseError.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
                    qWarning() << "Invalid JSON from DSN at index" << dsnIndex << ":" << jsonParseError.errorString();
                    continue;
                }

                const auto videos = jsonDocument.object()["videos"].toArray();

                for (const auto video : videos) {
                    auto videoMap = video.toVariant().toMap();

                    addVideo(new LearningCenterVideo(LearningCenterVideo::Type::YouTube, videoMap["title"].toString(), videoMap["tags"].toStringList(), videoMap["date"].toString().chopped(15), videoMap["summary"].toString(), videoMap["youtube-id"].toString()));
                }
            }

            emit populatedFromDsns();
            });

        _watcher.setFuture(_future);
        });

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        connect(&pluginFactory->getVideosDsnsAction(), &StringsAction::stringsChanged, this, &LearningCenterVideosModel::synchronizeWithDsns);
    }

    connect(&mv::settings().getAppFeaturesSettingsAction().getVideosAppFeatureAction(), &VideosAppFeatureAction::enabledChanged, this, &LearningCenterVideosModel::synchronizeWithDsns);
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

void LearningCenterVideosModel::synchronizeWithDsns()
{
    if (!mv::settings().getAppFeaturesSettingsAction().getVideosAppFeatureAction().getEnabledAction().isChecked())
        return;

    auto uniqueDsns = _dsnsAction.getStrings();

    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
        uniqueDsns << pluginFactory->getVideosDsnsAction().getStrings();
    }

    uniqueDsns.removeDuplicates();

    _dsnsAction.setStrings(uniqueDsns);
}

QByteArray LearningCenterVideosModel::downloadVideosFromDsn(const QString& dsn)
{
    QEventLoop loop;

    QByteArray downloadedData;

    FileDownloader fileDownloader;

    connect(&fileDownloader, &FileDownloader::downloaded, [&]() -> void {
        try
        {
            downloadedData = fileDownloader.downloadedData();
            loop.quit();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to download videos JSON from DSN", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to download videos JSON from DSN");
        }
        });

    fileDownloader.download(dsn);

    loop.exec();

    return downloadedData;
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