#pragma once

#include <QObject>
#include <QColor>

namespace hdps {

namespace gui {

class ColorMapEditor1DAction;

/**
 * Color map editor one-dimensional node class
 * 
 * Container class for one-dimension color map editor node
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class ColorMapEditor1DNode : public QObject
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param colorMapEditor1DAction Reference to owning one-dimensional color map editor
     * @param normalizedCoordinate Normalized node coordinates
     * @param color Node color
     */
    ColorMapEditor1DNode(ColorMapEditor1DAction& colorMapEditor1DAction, const QPointF& normalizedCoordinate, const QColor& color = Qt::gray);

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
     * Get movement limits
     * @return Limits rectangle
     */
    QRectF getLimits() const;

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
    ColorMapEditor1DAction&     _colorMapEditor1DAction;    /** Reference to owning one-dimensional color map editor */
    std::uint32_t               _index;                     /** Sorted node index */
    QPointF                     _normalizedCoordinate;      /** Normalized coordinate */
    QColor                      _color;                     /** Node color */
    float                       _radius;                    /** Node radius */
};

}
}
