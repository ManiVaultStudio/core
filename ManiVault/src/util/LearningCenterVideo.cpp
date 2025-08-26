// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningCenterVideo.h"

#include <QMovie>
#include <QFutureWatcher>

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
            const auto thumbnailUrl = getYouTubeThumbnailUrl(_resource);

            auto future = FileDownloader::downloadToByteArrayAsync(thumbnailUrl);

            auto watcher = new QFutureWatcher<QByteArray>(this);

            connect(watcher, &QFutureWatcher<QByteArray>::finished, watcher, [this, future, watcher, thumbnailUrl]() {
                try {
                    if (watcher->future().isCanceled() || watcher->future().isFinished() == false)
                        throw std::runtime_error("Future is cancelled or did not finish");

                    QMetaObject::invokeMethod(qApp, [this, future]() {
                        setThumbnailImage(QImage::fromData(future.result()));
					});
                }
                catch (const BaseException& exception) {
                    qCritical() << "Download video thumbnail image failed for" << thumbnailUrl << ":" << exception.what();
                }
                catch (const std::exception& exception) {
                    qCritical() << "Download video thumbnail image failed for" << thumbnailUrl << ":" << exception.what();
                }
                catch (...) {
                    qCritical() << "Download video thumbnail image failed for" << thumbnailUrl << "an unknown exception occurred";
                }

                watcher->deleteLater();
			});

            watcher->setFuture(future);

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
