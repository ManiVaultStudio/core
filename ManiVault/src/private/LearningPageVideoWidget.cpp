// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideoWidget.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/Miscellaneous.h>

#include <models/LearningCenterVideosModel.h>

#include <QDebug>

#include <QAbstractTextDocumentLayout>
#include <QDateTime>
#include <QDesktopServices>
#include <QEvent>
#include <QLocale>
#include <QModelIndex>
#include <QString>

//#define USE_YOUTUBE_DIALOG

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

LearningPageVideoWidget::LearningPageVideoWidget(const QModelIndex& index, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _index(index),
    _overlayWidget(index, &_thumbnailLabel)
{
    const auto title = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Title)).data().toString();

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
        _propertiesTextBrowser.setFixedHeight(static_cast<int>(_propertiesTextBrowser.document()->size().height()));
    });

    _overlayWidget.hide();

    _mainLayout.setContentsMargins(0, 0, 15, 0);
    _mainLayout.setSpacing(1);
    _mainLayout.setAlignment(Qt::AlignTop);

    _mainLayout.addWidget(&_thumbnailLabel);
    _mainLayout.addWidget(&_propertiesTextBrowser);

    setLayout(&_mainLayout);

    _thumbnailLabel.setObjectName("ThumbnailLabel");
    _thumbnailLabel.setFont(StyledIcon::getIconFont(20));
    _thumbnailLabel.setText(StyledIcon::getIconCharacter("spinner"));
    _thumbnailLabel.setAlignment(Qt::AlignCenter);
    _thumbnailLabel.setFixedSize(QSize(200, 102));
    _thumbnailLabel.setStyleSheet("QLabel#ThumbnailLabel { \
        background-color: rgb(200, 200, 200); \
        color: rgb(100, 100, 100); \
        border: 1px solid rgb(150, 150, 150); \
    }");

    const auto sourceModelIndex = dynamic_cast<const QSortFilterProxyModel*>(_index.model())->mapToSource(_index);
    const auto modelItem        = mv::help().getVideosModel().itemFromIndex(sourceModelIndex.siblingAtColumn(0));
    const auto videoModelItem   = dynamic_cast<LearningCenterVideosModel::Item*>(modelItem);
	const auto video            = videoModelItem->getVideo();

	const auto updateThumbnailImage = [this, video]() -> void {
        _thumbnailPixmap = QPixmap::fromImage(video->getThumbnailImage());

        constexpr auto  marginToRemove = 9;
        const auto      rectangleToCopy = QRect(0, marginToRemove, _thumbnailPixmap.width(), _thumbnailPixmap.height() - (2 * marginToRemove));

        _thumbnailPixmap = _thumbnailPixmap.copy(rectangleToCopy).scaledToWidth(200, Qt::SmoothTransformation);

        _thumbnailLabel.setFixedSize(_thumbnailPixmap.size());
        _thumbnailLabel.setPixmap(_thumbnailPixmap.copy());
    };

    if (video->hasThumbnailImage())
        updateThumbnailImage();
    else
        connect(video, &LearningCenterVideo::thumbnailImageReady, this, updateThumbnailImage);

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

LearningPageVideoWidget::OverlayWidget::OverlayWidget(const QModelIndex& index, QWidget* parent) :
    QWidget(parent),
    _index(index),
    _widgetOverlayer(this, this, parent)
{
    setObjectName("OverlayWidget");

    _mainLayout.setContentsMargins(4, 2, 4, 2);

    _mainLayout.addStretch(1);
    _mainLayout.addLayout(&_centerLayout, 1);
    _mainLayout.addLayout(&_bottomLayout, 1);

    _playIconLabel.setFont(StyledIcon::getIconFont(32));
    _summaryIconLabel.setFont(StyledIcon::getIconFont(8));
    _dateIconLabel.setFont(StyledIcon::getIconFont(8));
    _tagsIconLabel.setFont(StyledIcon::getIconFont(8));

    _playIconLabel.setText(StyledIcon::getIconCharacter("circle-play"));
    _summaryIconLabel.setText(StyledIcon::getIconCharacter("scroll"));
    _dateIconLabel.setText(StyledIcon::getIconCharacter("calendar-days"));
    _tagsIconLabel.setText(StyledIcon::getIconCharacter("tags"));
    
    QLocale locale;

    const auto summary      = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Summary)).data().toString();
    const auto date         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Date)).data(Qt::EditRole).toString();
    const auto dateTime     = QDateTime::fromString(date, Qt::ISODate);
    const auto dateString   = locale.toString(dateTime.date());
    const auto tags         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Tags)).data().toStringList();

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
                YouTubeVideoDialog::play(_index.sibling(_index.row(), static_cast<int>(HelpManagerVideosModel::Column::Resource)).data().toString());
#else
                QDesktopServices::openUrl(QString("https://www.youtube.com/watch?v=%1").arg(_index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Resource)).data().toString()));
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