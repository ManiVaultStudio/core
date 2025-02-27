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

    GraphicsItemFader(QGraphicsItem* item, qreal opacity = 0.f, QObject* parent = nullptr);

    void fadeIn(qreal opacity = 1.f, int duration = 300, int delay = 0);

    void fadeIn(int duration = 300, int delay = 0);

    void fadeOut(qreal opacity = 0.f, int duration = 600, int delay = 0);

    void fadeOut(int duration = 600, int delay = 0);

    void setOpacity(qreal opacity, int duration = 300, int delay = 0);

private:
    QGraphicsItem*          _item;              /** Pointer to the graphics item */
    QGraphicsOpacityEffect  _opacityEffect;     /** Opacity effect */
    QPropertyAnimation      _opacityAnimation;  /** Overlay animation */
};

class GraphicsIconItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:

    /**
     * Construct with icon name \p iconName, icon size \p iconSize and pointer to \p parent item
     * @param iconName Icon name
     * @param iconSize Icon size
     * @param intermediateOpacity Intermediate opacity
     * @param parent Parent graphics item
     */
    GraphicsIconItem(const QString& iconName, const QSize& iconSize, qreal intermediateOpacity = 0.5, QGraphicsItem* parent = nullptr);

    /**
     * Respond to hover enter events
     * @param event Hover event
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * Respond to hover move events
     * @param event Hover event
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * Respond to mouse press events
     * @param event Mouse event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * Returns the fader
     * @return Fader
     */
    GraphicsItemFader& getFader() { return _fader; }

signals:

    /** Signal emitted when the item is clicked */
	void clicked();

private:
    GraphicsItemFader   _fader;                 /** Fader for the item */
    qreal               _intermediateOpacity;   /** Intermediate opacity */
};

class OverlayGraphicsView : public QGraphicsView
{
public:
    explicit OverlayGraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent), targetWidget(parent) {
        setAttribute(Qt::WA_TransparentForMouseEvents, false);
        setStyleSheet("background: transparent;");
        setFrameStyle(QFrame::NoFrame);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override {
        QGraphicsItem* item = itemAt(event->pos());
        if (item) {
            // If an item is clicked, let QGraphicsView handle it
            QGraphicsView::mousePressEvent(event);
        }
        else {
            // Otherwise, forward the event to the underlying widget
            QApplication::sendEvent(targetWidget, event);
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        QGraphicsItem* item = itemAt(event->pos());
        if (item) {
            QGraphicsView::mouseMoveEvent(event);
        }
        else {
            QApplication::sendEvent(targetWidget, event);
        }
    }

    void mouseReleaseEvent(QMouseEvent* event) override {
        QGraphicsItem* item = itemAt(event->pos());
        if (item) {
            QGraphicsView::mouseReleaseEvent(event);
        }
        else {
            QApplication::sendEvent(targetWidget, event);
        }
    }

private:
    QWidget* targetWidget;
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
    OverlayGraphicsView         _overlayGraphicsView;       /** Overlay widget */
    QGraphicsRectItem           _backgroundItem;            /** Overlay background item */
    QGraphicsRectItem           _backgroundBorderItem;      /** Overlay background border item */
    QGraphicsLinearLayout       _verticalLayout;            /** Overlay vertical layout */
    QGraphicsLinearLayout       _metadataLayout;            /** Metadata layout */
    GraphicsIconItem            _playIconItem;              /** Play video item */
    GraphicsIconItem            _summaryIconItem;           /** Video summary item */
    GraphicsIconItem            _dateIconItem;              /** Video date item */
    GraphicsIconItem            _tagsIconItem;              /** Video tags item */
    QGraphicsWidget             _overlay;                   /** Overlay graphics widget */    
    GraphicsItemFader           _backgroundFader;           /** Background fader */
    GraphicsItemFader           _backgroundBorderFader;     /** Background border fader */

    friend class LearningPageVideoStyledItemDelegate;
};
