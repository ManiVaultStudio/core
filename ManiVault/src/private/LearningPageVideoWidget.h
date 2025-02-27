// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

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

/** Applies fading capabilities to a graphics item*/
class GraphicsItemFader : public QObject
{
public:

    /**
     * Construct with pointer to \p item, initial \p opacity and pointer to \p parent object
     * @param item Pointer to the graphics item
     * @param opacity Initial opacity
     * @param parent Pointer to parent object
     */
    GraphicsItemFader(QGraphicsItem* item, qreal opacity = 0.f, QObject* parent = nullptr);

    /**
     * Fade in the item with \p opacity, \p duration and \p delay
     * @param opacity Opacity
     * @param duration Duration
     * @param delay Delay
     */
    void fadeIn(qreal opacity = 1.f, int duration = 300, int delay = 0);

    /**
     * Fade in the item with \p duration and \p delay
     * @param duration Duration
     * @param delay Delay
     */
    void fadeIn(int duration = 300, int delay = 0);

    /**
     * Fade out the item with \p opacity, \p duration and \p delay
     * @param opacity Opacity
     * @param duration Duration
     * @param delay Delay
     */
    void fadeOut(qreal opacity = 0.f, int duration = 600, int delay = 0);

    /**
     * Fade out the item with \p duration and \p delay
     * @param duration Duration
     * @param delay Delay
     */
    void fadeOut(int duration = 600, int delay = 0);

    /**
     * Set the opacity of the item to \p opacity with \p duration and \p delay
     * @param opacity Opacity
     * @param duration Duration
     * @param delay Delay
     */
    void setOpacity(qreal opacity, int duration = 300, int delay = 0);

private:
    QGraphicsItem*          _item;              /** Pointer to the graphics item */
    QGraphicsOpacityEffect  _opacityEffect;     /** Opacity effect */
    QPropertyAnimation      _opacityAnimation;  /** Overlay animation */
};

/** Overlay the thumbnail with an icon */
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

/** Overlay the thumbnail with a rectangle */
class OverlayRectangleItem : public QGraphicsRectItem
{
public:

    /**
     * Construct with icon name \p iconName, icon size \p iconSize and pointer to \p parent item
     * @param parent Parent graphics item
     */
    OverlayRectangleItem(QGraphicsItem* parent = nullptr);

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
     * Returns the fader
     * @return Fader
     */
    GraphicsItemFader& getFader();

private:
    GraphicsItemFader   _fader;     /** Fader for the item */
};

/** Override to solve mouse event propagation issues */
class OverlayGraphicsView : public QGraphicsView
{
public:

	/**
	 * Create with pointer to \p parent widget
     * @param parent Pointer to parent widget
	 */
	explicit OverlayGraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent), _targetWidget(parent)
	{
        setFrameStyle(QFrame::NoFrame);
    }

protected:

    /**
     * Respond to mouse press events
     * @param event Mouse event
     */
    void mousePressEvent(QMouseEvent* event) override {
        if (auto item = itemAt(event->pos()))
            QGraphicsView::mousePressEvent(event);
        else
            QApplication::sendEvent(_targetWidget, event);
    }

    /**
     * Respond to mouse move events
     * @param event Mouse event
     */
    void mouseMoveEvent(QMouseEvent* event) override {
        if (auto item = itemAt(event->pos()))
            QGraphicsView::mouseMoveEvent(event);
        else
            QApplication::sendEvent(_targetWidget, event);
    }

    /**
     * Respond to mouse release events
     * @param event Mouse event
     */
    void mouseReleaseEvent(QMouseEvent* event) override {
        
        if (auto item = itemAt(event->pos()))
            QGraphicsView::mouseReleaseEvent(event);
        else
            QApplication::sendEvent(_targetWidget, event);
    }

private:
    QWidget* _targetWidget;  /** Pointer to the target widget */
};


/**
 * Learning page video widget class
 *
 * Show a learning page video
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
    OverlayRectangleItem        _overlayRectangleItem;      /** Overlay rectangle item */
    GraphicsIconItem            _playIconItem;              /** Play video item */
    GraphicsIconItem            _summaryIconItem;           /** Video summary item */
    GraphicsIconItem            _dateIconItem;              /** Video date item */
    GraphicsIconItem            _tagsIconItem;              /** Video tags item */
    QGraphicsWidget             _overlay;                   /** Overlay graphics widget */    

    friend class LearningPageVideoStyledItemDelegate;
};
