// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;
class ColorMapEditor1DNode;

/**
 * Color map editor one-dimensional node graphics item class
 *
 * Graphics item for drawing a node in a graphics view
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class ColorMapEditor1DNodeGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    /**
     * Constructor
     * @param colorMapEditor1DWidget Reference to owning one-dimensional color map editor widget
     * @param node Reference to source node
     */
    ColorMapEditor1DNodeGraphicsItem(ColorMapEditor1DWidget& colorMapEditor1DWidget, ColorMapEditor1DNode& node);

    /**
     * Get associated node
     * @return Reference to node
     */
    ColorMapEditor1DNode& getNode();

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Get the node bounding rectangle
     * @return Bounding rectangle
     */
    QRectF boundingRect() const override;

    /**
     * Get node shape
     * @return Shape as a painter path
     */
    QPainterPath shape() const override;

    /**
     * Paint the node
     * @param painter Pointer to painter
     * @param option Pointer to graphics item style options
     * @param widget Pointer to widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

    /**
     * Invoked when the node is pressed
     * @param event Pointer to graphics mouse event
     */
    void pressed(QGraphicsSceneMouseEvent* event);

    /**
     * Get the one-dimensional color map editor widget
     * @return Reference to one-dimensional color map editor widget
     */
    ColorMapEditor1DWidget& getColorMapEditor1DWidget();

protected:

    /**
     * Invoked when the mouse presses the node
     * @param event Pointer to graphics scene mouse event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * Invoked when the mouse moves over the node
     * @param event Pointer to graphics scene mouse event
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * Invoked when the mouse button releases
     * @param event Pointer to graphics scene mouse event
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /**
     * Invoked when the mouse starts to hover over the node
     * @param event Pointer to graphics scene hover event
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /**
     * Invoked when the mouse leaves the node after hovering
     * @param event Pointer to graphics scene hover event
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

signals:

    /**
     * Signals that the node color changed
     * @param color Node color
     */
    void colorChanged(const QColor& color);

    /**
     * Signals that the normalized node coordinates changed
     * @param normalizedCoordinate Normalized coordinate
     */
    void normalizedCoordinateChanged(const QPointF& normalizedCoordinate);

private:
    ColorMapEditor1DWidget&     _colorMapEditor1DWidget;    /** Reference to owning one-dimensional color map editor widget */
    ColorMapEditor1DNode&       _node;                      /** Reference to source node */
    bool                        _hover;                     /** Whether the mouse is hovering over the node */
};

}
}
