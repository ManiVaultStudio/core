#pragma once

#include <QGraphicsItem>

class QGraphicsSceneMouseEvent;

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;
class ColorMapEditor1DNode;
class ColorMapEditor1DEdge;

/**
 * Color map editor one-dimensional node graphics item class
 *
 * Visual node item for display in a graphics view
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class ColorMapEditor1DNodeGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

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

    void addEdge(ColorMapEditor1DEdge* edge);

    QVector<ColorMapEditor1DEdge*> edges() const;

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    QRectF boundingRect() const override;

    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void pressed(QGraphicsSceneMouseEvent* event);


    ColorMapEditor1DWidget& getColorMapEditor1DWidget() {
        return _colorMapEditor1DWidget;
    }
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * Invoked when the mouse over the node
     * @param graphicsSceneHoverEvent Pointer to graphics scene hover event
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent) override;

    /**
     * Invoked when the mouse leaves the node after hovering
     * @param graphicsSceneHoverEvent Pointer to graphics scene hover event
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* graphicsSceneHoverEvent) override;

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
    ColorMapEditor1DWidget&             _colorMapEditor1DWidget;    /** Reference to owning one-dimensional color map editor widget */
    ColorMapEditor1DNode&               _node;                      /** Reference to source node */
    QVector<ColorMapEditor1DEdge*>      _edgeList;                  /** Pointers to node edges */
    bool                                _hover;                     /** Whether the mouse is hovering over the node */
};

}
}
