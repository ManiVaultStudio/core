// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Video.h"

#include <QMovie>

using namespace mv::gui;

namespace mv::util {

Video::Video(const Type& type, const QString& title, const QStringList& tags, const QString& date, const QString& summary, const QString& resource) :
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

	            //constexpr auto  marginToRemove = 9;
	            //const auto      rectangleToCopy = QRect(0, marginToRemove, _thumbnailPixmap.width(), _thumbnailPixmap.height() - (2 * marginToRemove));

	            //_thumbnailPixmap = _thumbnailPixmap.copy(rectangleToCopy).scaledToWidth(200, Qt::SmoothTransformation);

	            //_thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
	            //_thumbnailLabel.setPixmap(_thumbnailPixmap.copy());

                
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

const Video::Type& Video::getType() const
{
    return _type;
}

const QString& Video::getTitle() const
{
    return _title;
}

const QStringList& Video::getTags() const
{
    return _tags;
}

const QString& Video::getDate() const
{
    return _date;
}

const QString& Video::getSummary() const
{
    return _summary;
}

const QString& Video::getResource() const
{
    return _resource;
}

QImage Video::getThumbnailImage() const
{
    return _thumbnailImage;
}

void Video::setThumbnailImage(const QImage& thumbnailImage)
{
    _thumbnailImage = thumbnailImage;

    emit thumbnailImageReady(_thumbnailImage);
}

bool Video::hasThumbnailImage() const
{
    return !_thumbnailImage.isNull();
}

QString Video::getYouTubeThumbnailUrl(const QString& videoId, const QString& quality /*= "mqdefault"*/)
{
    return QString("https://img.youtube.com/vi/%1/%2.jpg").arg(videoId, quality);
}

}
