// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QString>
#include <QStringList>
#include <QImage>

#include <vector>

#include "FileDownloader.h"

namespace mv::util {

/**
 * Video class
 *
 * Contains video meta data and offers ways to watch it
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Video : public QObject
{
    Q_OBJECT

public:
    /** Type of video */
    enum class Type {
	    YouTube,        /** Youtube video */
        GIF             /** GIF animation*/
    };

    using GifResource = QString;
    
    /**
     * Base constructor for use by type-specific constructors
     * @param type Type of video
     * @param title Video title
     * @param tags Video tags for filtering
     * @param date Issue date
     * @param summary Brief video description
     * @param resource Identifier of the YouTube or GIF
     */
    explicit Video(const Type& type, const QString& title, const QStringList& tags, const QString& date, const QString& summary, const QString& resource);

    /**
     * Get video type
     * @return Video type
     */
    const Type& getType() const;

    /**
     * Get video title
     * @return Video title
     */
    const QString& getTitle() const;

    /**
     * Get video tags
     * @return Video tags
     */
    const QStringList& getTags() const;

    /**
     * Get video date
     * @return Video date
     */
    const QString& getDate() const;

    /**
     * Get video summary (brief description)
     * @return Video summary
     */
    const QString& getSummary() const;

    /**
     * Get video resource
     * @return Video resource
     */
    const QString& getResource() const;

    /**
     * Get thumbnail image
     * @return Thumbnail image
     */
    QImage getThumbnailImage() const;

    /**
     * Set thumbnail image to /p thumbnailImage
     * @param thumbnailImage Thumbnail image
     */
    void setThumbnailImage(const QImage& thumbnailImage);

    /**
     * Get whether there is a valid thumbnail image
     * @return Boolean determining whether there is a valid thumbnail image
     */
    bool hasThumbnailImage() const;

    /**
     * Get youTube thumbnail for \p videoId with \p quality
     * @param videoId Globally unique identifier of the video
     * @param quality String determining the quality: "default", "hqdefault", "mqdefault", "sddefault", "maxresdefault"
     */
    static QString getYouTubeThumbnailUrl(const QString& videoId, const QString& quality = "mqdefault");

    /**
     * Overload assignment operator to avoid the watch action being copied
     * @param rhs Right hand side video
     * @return Assigned
     */
    Video& operator=(const Video& rhs)
    {
        _type           = rhs.getType();
        _title          = rhs.getTitle();
        _tags           = rhs.getTags();
        _date           = rhs.getDate();
        _summary        = rhs.getSummary();
        _resource       = rhs.getResource();

        return *this;
    }

signals:

    /**
     * Signals that /p thumbnailImage is ready for use
     * @param thumbnailImage Thumbnail image
     */
    void thumbnailImageReady(const QImage& thumbnailImage);

private:
    Type            _type;                      /** Type */
    QString         _title;                     /** Title */
    QStringList     _tags;                      /** Tags */
    QString         _date;                      /** Date and time */
    QString         _summary;                   /** Summary */
    QString         _resource;                  /** Video resource */
    FileDownloader  _thumbnailDownloader;       /** File downloader for downloading the YouTube thumbnail image */
    QImage          _thumbnailImage;            /** Cached thumbnail image */
};

using Videos = std::vector<const Video*>;

}
