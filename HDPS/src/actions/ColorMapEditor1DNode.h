#pragma once

#include <QObject>
#include <QGraphicsItem>
#include <QCursor>

class QGraphicsSceneMouseEvent;

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;
class ColorMapEditor1DEdge;

class ColorMapEditor1DNode : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    ColorMapEditor1DNode(ColorMapEditor1DWidget& colorMapEditor1DWidget);

    /**
     * Get sorted node index
     * @return Sorted node index
     */
    std::uint32_t getIndex() const;

    /**
     * Set sorted node index
     * @param index Sorted node index
     */
    void setIndex(const std::uint32_t& index);

    /**
     * Get node color
     * @return Node color
     */
    QColor getColor() const;

    /**
     * Set node color
     * @param color Node color
     */
    void setColor(const QColor& color);

    /**
     * Get node radius
     * @return Node radius
     */
    float getRadius() const;

    /**
     * Set node radius
     * @param radius Node radius
     */
    void setRadius(const float& radius);

    /**
     * Get normalized node coordinates
     * @return Normalized node coordinates
     */
    QPointF getNormalizedCoordinate() const;

    /**
     * Set normalized node coordinates
     * @return Normalized node coordinate
     */
    void setNormalizedCoordinate(const QPointF& normalizedCoordinate);

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

    /**
     * Get movement limits in graph coordinates
     * @return Limits rectangle
     */
    QRectF getLimits() const;

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
    std::uint32_t                       _index;                     /** Sorted node index */
    QColor                              _color;                     /** Node color */
    float                               _radius;                    /** Node radius */
    QVector<ColorMapEditor1DEdge*>      _edgeList;                  /** Pointers to node edges */
    bool                                _hover;                     /** Whether the mouse is hovering over the node */
};

}
}
