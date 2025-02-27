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
#include <QToolTip>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneHelpEvent>

//#define USE_YOUTUBE_DIALOG

using namespace mv;
using namespace mv::gui;
using namespace mv::util;

GraphicsItemFader::GraphicsItemFader(QGraphicsItem* item, qreal opacity, QObject* parent):
	QObject(parent),
	_item(item),
	_opacityEffect(this),
	_opacityAnimation(&_opacityEffect, "opacity")
{
	_item->setGraphicsEffect(&_opacityEffect);

	fadeIn(opacity);
}

void GraphicsItemFader::fadeIn(qreal opacity, int duration, int delay)
{
	return setOpacity(opacity, duration, delay);
}

void GraphicsItemFader::fadeIn(int duration, int delay)
{
	return setOpacity(1.0, duration, delay);
}

void GraphicsItemFader::fadeOut(qreal opacity, int duration, int delay)
{
	return setOpacity(opacity, duration, delay);
}

void GraphicsItemFader::fadeOut(int duration, int delay)
{
	return setOpacity(0.0, duration, delay);
}

void GraphicsItemFader::setOpacity(qreal opacity, int duration, int delay)
{
	if (_opacityEffect.opacity() == opacity)
		return;

    const auto animateOpacity = [this, duration, opacity]() -> void {
        _opacityAnimation.stop();
        _opacityAnimation.setDuration(duration);
        _opacityAnimation.setStartValue(_opacityAnimation.currentValue());
        _opacityAnimation.setEndValue(opacity);

        _opacityAnimation.start();
	};

    if (delay > 0) {
        QTimer::singleShot(delay, animateOpacity);
    } else {
        animateOpacity();
    }
}

GraphicsIconItem::GraphicsIconItem(const QString& iconName, const QSize& iconSize, qreal intermediateOpacity /*= 0.5*/, QGraphicsItem* parent /*= nullptr*/):
	QGraphicsPixmapItem(QIcon(StyledIcon(iconName)).pixmap(iconSize), parent),
	_fader(this),
    _intermediateOpacity(intermediateOpacity)
{
	setAcceptHoverEvents(true);
}

void GraphicsIconItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
	_fader.fadeIn(1.0, 150);
}

void GraphicsIconItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
	_fader.setOpacity(_intermediateOpacity);
}

void GraphicsIconItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsPixmapItem::mousePressEvent(event);

    emit clicked();
}

LearningPageVideoWidget::LearningPageVideoWidget(const QModelIndex& index, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _index(index),
    _overlayGraphicsScene(this),
    _overlayGraphicsView(this),
    _verticalLayout(Qt::Vertical),
	_metadataLayout(Qt::Horizontal),
	_playIconItem("play", { 32, 32 }, 0.25),
	_summaryIconItem("scroll", { 12, 12 }, 0.15),
	_dateIconItem("calendar", { 12, 12 }, 0.15),
	_tagsIconItem("tags", { 12, 12 }, 0.15),
    _backgroundFader(&_backgroundItem, 0.2),
    _backgroundBorderFader(&_backgroundBorderItem)
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

    _mainLayout.setContentsMargins(0, 0, 0, 0);
    //_mainLayout.setSpacing(1);
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

    _backgroundItem.setRect(QRect(QPoint(0, 0), QSize(200, 102)));
    _backgroundBorderItem.setRect(QRect(QPoint(0, 1), QSize(200, 102) - QSize(1, 2)));

    _playIconItem.setPos(100 - 16, 50 - 16);
    _summaryIconItem.setPos(140, 80);
    _dateIconItem.setPos(160, 80);
    _tagsIconItem.setPos(180, 80);

    connect(&_playIconItem, &GraphicsIconItem::clicked, this, [this]() -> void {
#ifdef USE_YOUTUBE_DIALOG
        YouTubeVideoDialog::play(_index.sibling(_index.row(), static_cast<int>(HelpManagerVideosModel::Column::Resource)).data().toString());
#else
        QDesktopServices::openUrl(QString("https://www.youtube.com/watch?v=%1").arg(_index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Resource)).data().toString()));
#endif
	});

    QLocale locale;

    const auto summary      = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Summary)).data().toString();
    const auto date         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Date)).data(Qt::EditRole).toString();
    const auto dateTime     = QDateTime::fromString(date, Qt::ISODate);
    const auto dateString   = locale.toString(dateTime.date());
    const auto tags         = _index.sibling(_index.row(), static_cast<int>(LearningCenterVideosModel::Column::Tags)).data().toStringList();

    _playIconItem.setToolTip("Play the video on YouTube");
    _summaryIconItem.setToolTip(summary);
    _dateIconItem.setToolTip(dateString);
    _tagsIconItem.setToolTip(tags.join(", "));

    _overlayGraphicsScene.addItem(&_backgroundItem);
    _overlayGraphicsScene.addItem(&_backgroundBorderItem);
    _overlayGraphicsScene.addItem(&_playIconItem);
    _overlayGraphicsScene.addItem(&_summaryIconItem);
    _overlayGraphicsScene.addItem(&_dateIconItem);
    _overlayGraphicsScene.addItem(&_tagsIconItem);

    _overlayGraphicsView.setObjectName("OverlayGraphicsView");
    _overlayGraphicsView.setRenderHint(QPainter::Antialiasing);
    _overlayGraphicsView.setRenderHint(QPainter::SmoothPixmapTransform);
    _overlayGraphicsView.setStyleSheet("background: transparent; border: none;");
    _overlayGraphicsView.setAttribute(Qt::WA_TranslucentBackground);
    //_overlayGraphicsView.setAttribute(Qt::WA_TransparentForMouseEvents);
    _overlayGraphicsView.show();
    _overlayGraphicsView.raise();
    _overlayGraphicsView.setGeometry(0, 0, 200, 101);
    _overlayGraphicsView.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _overlayGraphicsView.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    updateCustomStyle();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, &LearningPageVideoWidget::updateCustomStyle);

    installEventFilter(this);
}

bool LearningPageVideoWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            _backgroundFader.fadeIn(250);
            _backgroundBorderFader.fadeIn(250);

            _playIconItem.getFader().fadeIn(0.25, 250, 200);
            _summaryIconItem.getFader().fadeIn(0.15, 250, 300);
            _dateIconItem.getFader().fadeIn(0.15, 250, 350);
            _tagsIconItem.getFader().fadeIn(0.15, 250, 400);

            break;
        }

        case QEvent::Leave:
        {
            _backgroundFader.fadeOut(250, 450);
            _backgroundBorderFader.fadeOut(250, 450);

            _playIconItem.getFader().fadeOut(250, 400);
            _summaryIconItem.getFader().fadeOut(200, 300);
            _dateIconItem.getFader().fadeOut(200, 250);
            _tagsIconItem.getFader().fadeOut(200, 200);

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
        border: 1px solid %1; \
    }").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window).name()));

    _propertiesTextBrowser.setStyleSheet(QString("background-color: transparent; color: %1; border: none; margin: 0px; padding: 0px;").arg(qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text).name()));

    auto windowBackgroundColor = qApp->palette().color(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window);

    windowBackgroundColor.setAlphaF(1.f);

    _backgroundItem.setBrush(windowBackgroundColor);

    _backgroundBorderItem.setBrush(Qt::NoBrush);
    _backgroundBorderItem.setPen(QPen(qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Accent), 1));
}
