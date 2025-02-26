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
#include <QGraphicsPixmapItem>

//#define USE_YOUTUBE_DIALOG

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

LearningPageVideoWidget::LearningPageVideoWidget(const QModelIndex& index, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _index(index),
    _overlayGraphicsScene(this),
    _overlayGraphicsView(this),
    _verticalLayout(Qt::Vertical),
	_metadataLayout(Qt::Horizontal),
    _backgroundFader(&_backgroundItem, 0.2),
    _backgroundBorderFader(&_backgroundBorderItem),
    _playItemFader(&_playItem),
    _summaryItemFader(&_summaryItem),
    _dateItemFader(&_dateItem),
    _tagsItemFader(&_tagsItem)
{
    const auto title = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Title)).data().toString();

    _propertiesTextBrowser.setReadOnly(true);
    _propertiesTextBrowser.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _propertiesTextBrowser.document()->setDocumentMargin(0);
    _propertiesTextBrowser.setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    _propertiesTextBrowser.setFixedWidth(200);
    _propertiesTextBrowser.setHtml(QString("<p style='margin-top: 0px;'><b>%1</b></p>").arg(title));

    connect(_propertiesTextBrowser.document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged, this, [this]() -> void {
        _propertiesTextBrowser.setFixedHeight(static_cast<int>(_propertiesTextBrowser.document()->size().height()));
    });

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

    _overlayGraphicsScene.setSceneRect(QRect(QPoint(0, 0), QSize(200, 102)));

    _overlayGraphicsView.setScene(&_overlayGraphicsScene);
    _overlayGraphicsView.setGeometry(0, 0, width(), height());

    _backgroundItem.setToolTip("asdasdasdasd");
    _backgroundItem.setRect(QRect(QPoint(0, 0), QSize(200, 102)));
    _backgroundBorderItem.setRect(QRect(QPoint(0, 1), QSize(200, 102) - QSize(1, 2)));

    _playItem.setPixmap(QIcon(StyledIcon("play")).pixmap(32, 32));
    _summaryItem.setPixmap(QIcon(StyledIcon("scroll")).pixmap(12, 12));
    _dateItem.setPixmap(QIcon(StyledIcon("calendar")).pixmap(12, 12));
    _tagsItem.setPixmap(QIcon(StyledIcon("tags")).pixmap(12, 12));

    _playItem.setPos(100 - 16, 50 - 16);
    _summaryItem.setPos(140, 80);
    _dateItem.setPos(160, 80);
    _tagsItem.setPos(180, 80);

    QLocale locale;

    const auto summary      = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Summary)).data().toString();
    const auto date         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Date)).data(Qt::EditRole).toString();
    const auto dateTime     = QDateTime::fromString(date, Qt::ISODate);
    const auto dateString   = locale.toString(dateTime.date());
    const auto tags         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Tags)).data().toStringList();

    _playItem.setToolTip("Play the video on YouTube");
    _summaryItem.setToolTip(summary);
    _dateItem.setToolTip(dateString);
    _tagsItem.setToolTip(tags.join(", "));

    _overlayGraphicsScene.addItem(&_backgroundItem);
    _overlayGraphicsScene.addItem(&_backgroundBorderItem);
    _overlayGraphicsScene.addItem(&_playItem);
    _overlayGraphicsScene.addItem(&_summaryItem);
    _overlayGraphicsScene.addItem(&_dateItem);
    _overlayGraphicsScene.addItem(&_tagsItem);

    _overlayGraphicsView.setRenderHint(QPainter::Antialiasing);
    _overlayGraphicsView.setRenderHint(QPainter::SmoothPixmapTransform);
    _overlayGraphicsView.setStyleSheet("background: transparent; border: none;");
    _overlayGraphicsView.setAttribute(Qt::WA_TranslucentBackground);
    _overlayGraphicsView.setAttribute(Qt::WA_TransparentForMouseEvents);
    _overlayGraphicsView.show();
    _overlayGraphicsView.raise();
    _overlayGraphicsView.setGeometry(0, 0, 200, 101);
    _overlayGraphicsView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _overlayGraphicsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateCustomStyle();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &LearningPageVideoWidget::updateCustomStyle);

    _thumbnailLabel.installEventFilter(this);
    _thumbnailLabel.setMouseTracking(true);
}

bool LearningPageVideoWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
		case QEvent::MouseMove:
	    {
            _playItemFader.setOpacity(_playItem.isUnderMouse() ? 1.0 : 0.25);
            _summaryItemFader.setOpacity(_summaryItem.isUnderMouse() ? 1.0 : 0.15);
            _dateItemFader.setOpacity(_dateItem.isUnderMouse() ? 1.0 : 0.15);
            _tagsItemFader.setOpacity(_tagsItem.isUnderMouse() ? 1.0 : 0.15);

            break;
	    }

        case QEvent::Enter:
        {
            _backgroundFader.fadeIn(0.9, 150);
            _backgroundBorderFader.fadeIn(1, 150);
            _playItemFader.fadeIn(0.25, 150);
            _summaryItemFader.fadeIn(0.15, 150);
            _dateItemFader.fadeIn(0.15, 150);
            _tagsItemFader.fadeIn(0.15, 150);

            break;
        }

        case QEvent::Leave:
        {
            _backgroundFader.fadeOut(600);
            _backgroundBorderFader.fadeOut(600);
            _playItemFader.fadeOut(400);
            _summaryItemFader.fadeOut(400);
            _dateItemFader.fadeOut(400);
            _tagsItemFader.fadeOut(400);

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void LearningPageVideoWidget::updateCustomStyle()
{
    _thumbnailLabel.setStyleSheet(QString("QLabel#ThumbnailLabel { \
        background-color: rgb(200, 200, 200); \
        color: rgb(100, 100, 100); \
        border: 1px solid %1; \
    }").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window).name()));

    _propertiesTextBrowser.setStyleSheet(QString("background-color: transparent; color: %1; border: none; margin: 0px; padding: 0px;").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text).name()));

    auto windowBackgroundColor = qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window);

    windowBackgroundColor.setAlphaF(1.f);

    _backgroundItem.setBrush(windowBackgroundColor);

    _backgroundBorderItem.setBrush(Qt::NoBrush);
    _backgroundBorderItem.setPen(QPen(qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Accent), 1));
}
