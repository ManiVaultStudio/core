// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterVideo.h"

#include <QMovie>

namespace mv::util {

LearningCenterVideo::LearningCenterVideo(const Type& type, const QString& title, const QStringList& tags, const QString& date, const QString& summary, const QString& resource) :
    _type(type),
    _title(title),
    _tags(tags),
    _date(date),
    _summary(summary),
    _resource(resource)
{
    switch (_type) {
	    case Type::YouTube:
	    {
	        connect(&_thumbnailDownloader, &FileDownloader::downloaded, this, [this]() -> void {
                setThumbnailImage(QImage::fromData(_thumbnailDownloader.downloadedData()));
			});

	        const auto youTubeThumbnailUrl = getYouTubeThumbnailUrl(_resource);

	        _thumbnailDownloader.download(QUrl(youTubeThumbnailUrl));

	        break;
	    }

        case Type::GIF:
        {
            QMovie movie(_resource);

            if (movie.isValid()) {
                movie.jumpToFrame(0);

                setThumbnailImage(movie.currentImage());
            }

            break;
        }
    }
}

LearningCenterVideo::LearningCenterVideo(const Type& type, const QVariantMap& variantMap) :
    _type(type),
    _title(variantMap["title"].toString()),
    _tags(variantMap["tags"].toStringList()),
    _date(variantMap["date"].toString()),
    _summary(variantMap["summary"].toString()),
    _resource(variantMap["resource"].toString())
{
}

const LearningCenterVideo::Type& LearningCenterVideo::getType() const
{
    return _type;
}

const QString& LearningCenterVideo::getTitle() const
{
    return _title;
}

const QStringList& LearningCenterVideo::getTags() const
{
    return _tags;
}

const QString& LearningCenterVideo::getDate() const
{
    return _date;
}

const QString& LearningCenterVideo::getSummary() const
{
    return _summary;
}

const QString& LearningCenterVideo::getResource() const
{
    return _resource;
}

QImage LearningCenterVideo::getThumbnailImage() const
{
    return _thumbnailImage;
}

void LearningCenterVideo::setThumbnailImage(const QImage& thumbnailImage)
{
    _thumbnailImage = thumbnailImage;

    emit thumbnailImageReady(_thumbnailImage);
}

bool LearningCenterVideo::hasThumbnailImage() const
{
    return !_thumbnailImage.isNull();
}

QString LearningCenterVideo::getYouTubeThumbnailUrl(const QString& videoId, const QString& quality /*= "mqdefault"*/)
{
    return QString("https://img.youtube.com/vi/%1/%2.jpg").arg(videoId, quality);
}

}
