// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <util/WidgetOverlayer.h>

#include <QLabel>
#include <QPersistentModelIndex>
#include <QPixmap>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsRectItem>
#include <QGraphicsLinearLayout>

class GraphicsItemFader : public QObject
{
public:

    GraphicsItemFader(QGraphicsItem* item, qreal opacity = 0.f, QObject* parent = nullptr) :
        QObject(parent),
        _item(item),
        _opacityEffect(this),
        _opacityAnimation(&_opacityEffect, "opacity")
    {
        _item->setGraphicsEffect(&_opacityEffect);

        fadeIn(opacity);
    }

    void fadeIn(qreal opacity = 1.f, int duration = 300) {
        setOpacity(opacity, duration);
    }

    void fadeIn(int duration = 300) {
        setOpacity(1.0, duration);
    }

    void fadeOut(qreal opacity = 0.f, int duration = 300) {
        setOpacity(opacity, duration);
    }

    void fadeOut(int duration = 300) {
        setOpacity(0.0, duration);
    }

    void setOpacity(qreal opacity, int duration = 300) {
        if (_opacityEffect.opacity() == opacity)
            return;

        _opacityAnimation.setDuration(duration);
        _opacityAnimation.stop();
        _opacityAnimation.setStartValue(_opacityAnimation.currentValue());
        _opacityAnimation.setEndValue(opacity);

        _opacityAnimation.start();
    }

private:
    QGraphicsItem*          _item;              /** Pointer to the graphics item */
    QGraphicsOpacityEffect  _opacityEffect;     /** Opacity effect */
    QPropertyAnimation      _opacityAnimation;  /** Overlay animation */
};

/**
 * Learning page video widget class
 *
 * Widget class which show a learning page video
 *
 * @author Thomas Kroes
 */
class LearningPageVideoWidget : public QWidget
{
public:

    /**
     * Construct with model \p index and pointer to \p parent widget
     * @param index Model index for videos model
     * @param parent Pointer to parent widget
     */
    LearningPageVideoWidget(const QModelIndex& index, QWidget* parent = nullptr);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Updates custom styling */
    void updateCustomStyle();

private:
    QPersistentModelIndex       _index;                     /** Pointer to owning learning page content widget */
    QVBoxLayout                 _mainLayout;                /** Main vertical layout */
    QLabel                      _thumbnailLabel;            /** Label that shows the thumbnail pixmap */
    QPixmap                     _thumbnailPixmap;           /** Thumbnail pixmap */
    QTextBrowser                _propertiesTextBrowser;     /** Text browser for showing the video title */
    QGraphicsScene              _overlayGraphicsScene;      /** Overlay graphics scene */
    QGraphicsView               _overlayGraphicsView;       /** Overlay widget */
    QGraphicsRectItem           _backgroundItem;            /** Overlay background item */
    QGraphicsRectItem           _backgroundBorderItem;      /** Overlay background border item */
    QGraphicsLinearLayout       _verticalLayout;            /** Overlay vertical layout */
    QGraphicsLinearLayout       _metadataLayout;            /** Metadata layout */
    QGraphicsPixmapItem         _playItem;                  /** Play video item */
    QGraphicsPixmapItem         _summaryItem;               /** Video summary item */
    QGraphicsPixmapItem         _dateItem;                  /** Video date item */
    QGraphicsPixmapItem         _tagsItem;                  /** Video tags item */
    QGraphicsWidget             _overlay;                   /** Overlay graphics widget */    
    GraphicsItemFader           _backgroundFader;           /** Background fader */
    GraphicsItemFader           _backgroundBorderFader;     /** Background border fader */
    GraphicsItemFader           _playItemFader;             /** Play item fader */
    GraphicsItemFader           _summaryItemFader;          /** Summary metadata item fader */
    GraphicsItemFader           _dateItemFader;             /** Date metadata item fader */
    GraphicsItemFader           _tagsItemFader;             /** Tags metadata item fader */


    friend class LearningPageVideoStyledItemDelegate;
};
