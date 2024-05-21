// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoWidget.h"
#include "LearningPageVideosModel.h"

#include <Application.h>

#include <util/Miscellaneous.h>

#include <widgets/YouTubeVideoDialog.h>

#include <QDebug>
#include <QAbstractTextDocumentLayout>
#include <QEvent>
#include <QDesktopServices>

//#define USE_YOUTUBE_DIALOG

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

LearningPageVideoWidget::LearningPageVideoWidget(const QModelIndex& index, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _index(index),
    _mainLayout(),
    _thumbnailLabel(),
    _thumbnailPixmap(),
    _thumbnailDownloader(),
    _propertiesTextBrowser(),
    _overlayWidget(index, &_thumbnailLabel)
{
    const auto title = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Title)).data().toString();

    _propertiesTextBrowser.setReadOnly(true);
    _propertiesTextBrowser.setStyleSheet("background-color: transparent; border: none; margin: 0px; padding: 0px;");
    _propertiesTextBrowser.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.document()->setDocumentMargin(0);
    _propertiesTextBrowser.setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    _propertiesTextBrowser.setFixedWidth(200);
    _propertiesTextBrowser.setHtml(QString("<p style='margin-top: 0px; rgb(75, 75, 75)'><b>%1</b></p>").arg(title));
    _propertiesTextBrowser.setStyleSheet(" \
        background-color: transparent; \
        border: none; \
        margin: 0px; \
        padding: 0px; \
    ");

    connect(_propertiesTextBrowser.document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [this]() -> void {
        _propertiesTextBrowser.setFixedHeight(_propertiesTextBrowser.document()->size().height());
    });

    _overlayWidget.hide();

    _mainLayout.setContentsMargins(0, 0, 15, 0);
    _mainLayout.setSpacing(1);
    _mainLayout.setAlignment(Qt::AlignTop);

    _mainLayout.addWidget(&_thumbnailLabel);
    _mainLayout.addWidget(&_propertiesTextBrowser);

    setLayout(&_mainLayout);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _thumbnailLabel.setObjectName("ThumbnailLabel");
    _thumbnailLabel.setFont(fontAwesome.getFont(20));
    _thumbnailLabel.setText(fontAwesome.getIconCharacter("spinner"));
    _thumbnailLabel.setAlignment(Qt::AlignCenter);
    _thumbnailLabel.setFixedSize(QSize(200, 102));
    _thumbnailLabel.setStyleSheet("QLabel#ThumbnailLabel { \
        background-color: rgb(200, 200, 200); \
        color: rgb(100, 100, 100); \
        border: 1px solid rgb(150, 150, 150); \
    }");

    connect(&_thumbnailDownloader, &FileDownloader::downloaded, this, [this]() -> void {
        _thumbnailPixmap = QPixmap::fromImage(QImage::fromData(_thumbnailDownloader.downloadedData()));

        const auto marginToRemove   = 9;
        const auto rectangleToCopy  = QRect(0, marginToRemove, _thumbnailPixmap.width(), _thumbnailPixmap.height() - (2 * marginToRemove));

        _thumbnailPixmap = _thumbnailPixmap.copy(rectangleToCopy).scaledToWidth(200, Qt::SmoothTransformation);

        _thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
        _thumbnailLabel.setPixmap(_thumbnailPixmap.copy());
    });

    const auto youTubeId            = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString();
    const auto youtTubeThumbnailUrl = getYouTubeThumbnailUrl(youTubeId);

    _thumbnailDownloader.download(QUrl(youtTubeThumbnailUrl));

    _thumbnailLabel.installEventFilter(this);
}

bool LearningPageVideoWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            if (target == &_thumbnailLabel) {
                _overlayWidget.show();
                _overlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            }
            
            break;
        }

        case QEvent::Leave:
        {
            if (target == &_thumbnailLabel) {
                _overlayWidget.hide();
                _overlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
            }

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

QString LearningPageVideoWidget::getYouTubeThumbnailUrl(const QString& videoId, const QString& quality /*= "mqdefault"*/)
{
    return QString("https://img.youtube.com/vi/%1/%2.jpg").arg(videoId, quality);
}

LearningPageVideoWidget::OverlayWidget::OverlayWidget(const QModelIndex& index, QWidget* parent) :
    QWidget(parent),
    _index(index),
    _mainLayout(),
    _centerLayout(),
    _bottomLayout(),
    _playIconLabel(),
    _summaryIconLabel(),
    _dateIconLabel(),
    _tagsIconLabel(),
    _widgetOverlayer(this, this, parent)
{
    setObjectName("OverlayWidget");

    _mainLayout.setContentsMargins(4, 2, 4, 2);

    _mainLayout.addStretch(1);
    _mainLayout.addLayout(&_centerLayout, 1);
    _mainLayout.addLayout(&_bottomLayout, 1);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _playIconLabel.setFont(fontAwesome.getFont(32));
    _summaryIconLabel.setFont(fontAwesome.getFont(8));
    _dateIconLabel.setFont(fontAwesome.getFont(8));
    _tagsIconLabel.setFont(fontAwesome.getFont(8));

    _playIconLabel.setText(fontAwesome.getIconCharacter("play-circle"));
    _summaryIconLabel.setText(fontAwesome.getIconCharacter("scroll"));
    _dateIconLabel.setText(fontAwesome.getIconCharacter("calendar-alt"));
    _tagsIconLabel.setText(fontAwesome.getIconCharacter("tags"));
    
    QLocale locale;

    const auto summary      = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Summary)).data().toString();
    const auto date         = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Date)).data(Qt::EditRole).toString();
    const auto dateTime     = QDateTime::fromString(date, Qt::ISODate);
    const auto dateString   = locale.toString(dateTime.date());
    const auto tags         = _index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::Tags)).data().toStringList();

    _playIconLabel.setToolTip("Click to start the video");
    _summaryIconLabel.setToolTip(summary);
    _dateIconLabel.setToolTip(dateString);
    _tagsIconLabel.setToolTip(QString("%1").arg(tags.join(", ")));

    _centerLayout.setAlignment(Qt::AlignCenter);

    _centerLayout.addWidget(&_playIconLabel);

    _bottomLayout.addStretch(1);

    _bottomLayout.addWidget(&_summaryIconLabel);
    _bottomLayout.addWidget(&_dateIconLabel);
    _bottomLayout.addWidget(&_tagsIconLabel);

    auto backgroundColor = QApplication::palette().color(QPalette::Normal, QPalette::Midlight);

    backgroundColor.setAlpha(200);

    setStyleSheet("QWidget#OverlayWidget { \
        background-color: rgba(230, 230, 230, 230); \
        border: 1px solid rgb(150, 150, 150); \
    }");

    setLayout(&_mainLayout);

    _playIconLabel.installEventFilter(this);
    _summaryIconLabel.installEventFilter(this);
    _dateIconLabel.installEventFilter(this);
    _tagsIconLabel.installEventFilter(this);

    updateStyle();
}

bool LearningPageVideoWidget::OverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (target == &_playIconLabel) {
#ifdef USE_YOUTUBE_DIALOG
                YouTubeVideoDialog::play(_index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeId)).data().toString());
#else
                QDesktopServices::openUrl(_index.sibling(_index.row(), static_cast<int>(LearningPageVideosModel::Column::YouTubeUrl)).data().toString());
#endif
            }

            break;
        }

        case QEvent::Enter:
        case QEvent::Leave:
            updateStyle();
            break;

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void LearningPageVideoWidget::OverlayWidget::updateStyle()
{
    auto colorEnter = getColorAsCssString(QColor(0, 0, 0, 150));
    auto colorLeave = getColorAsCssString(QColor(0, 0, 0, 70));

    _playIconLabel.setStyleSheet(QString("color: %1").arg(_playIconLabel.underMouse() ? colorEnter : colorLeave));
    _summaryIconLabel.setStyleSheet(QString("color: %1").arg(_summaryIconLabel.underMouse() ? colorEnter : colorLeave));
    _dateIconLabel.setStyleSheet(QString("color: %1").arg(_dateIconLabel.underMouse() ? colorEnter : colorLeave));
    _tagsIconLabel.setStyleSheet(QString("color: %1").arg(_tagsIconLabel.underMouse() ? colorEnter : colorLeave));
}