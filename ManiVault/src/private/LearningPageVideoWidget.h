// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QTextBrowser>

#include "util/FileDownloader.h"

class LearningPageVideosWidget;

/**
 * Learning page video widget class
 *
 * Widget class which show a learning page video
 *
 * @author Thomas Kroes
 */
class LearningPageVideoWidget : public QWidget
{
private:

    class OverlayWidget : public QWidget {
    public:
        explicit OverlayWidget(const QModelIndex& index, QWidget* parent);

        /**
         * Respond to target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        void updateStyle();

    private:
        QPersistentModelIndex       _index;                     /** Pointer to owning learning page content widget */
        QVBoxLayout                 _mainLayout;                /** Main vertical layout */
        QHBoxLayout                 _centerLayout;              /** Center horizontal layout for the play button */
        QHBoxLayout                 _bottomLayout;              /** Bottom horizontal layout for the meta labels */
        QLabel                      _playIconLabel;             /** Play label */
        QLabel                      _summaryIconLabel;          /** Play label */
        QLabel                      _dateIconLabel;             /** Date label */
        QLabel                      _tagsIconLabel;             /** Tags label */
        mv::util::WidgetOverlayer   _widgetOverlayer;           /** Synchronizes the size with the source widget */
    };

protected:

    /**
     * Construct with model \p index and pointer to \p learningPageContentWidget
     * @param index Model index for videos model
     * @param learningPageContentWidget Pointer to owning learning page content widget
     */
    LearningPageVideoWidget(const QModelIndex& index, QWidget* parent = nullptr);

    /**
     * Triggered on mouse hover
     * @param enterEvent Pointer to enter event
     */
    void enterEvent(QEnterEvent* enterEvent) override;

    /**
     * Triggered on mouse leave
     * @param leaveEvent Pointer to leave event
     */
    void leaveEvent(QEvent* leaveEvent) override;

    /**
     * Get youTube thumbnail for \p videoId with \p quality
     * @param videoId Globally unique identifier of the video
     * @param quality String determining the quality: "default", "hqdefault", "mqdefault", "sddefault", "maxresdefault"
     */
    static QString getYouTubeThumbnailUrl(const QString& videoId, const QString& quality = "mqdefault");

private:
    QPersistentModelIndex       _index;         /** Pointer to owning learning page content widget */
    QVBoxLayout                 _mainLayout;    /** Main vertical layout */
    QLabel                      _thumbnailLabel;
    QPixmap                     _thumbnailPixmap;
    mv::util::FileDownloader    _thumbnailDownloader;
    QTextBrowser                _propertiesTextBrowser;
    OverlayWidget               _overlayWidget;

    friend class LearningPageVideosWidget;
};