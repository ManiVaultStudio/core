#pragma once

#include <QGraphicsScene>
#include <QCursor>

class QGraphicsSceneMouseEvent;

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;

class ColorMapEditor1DScene : public QGraphicsScene
{
    Q_OBJECT
public:

    /**
     * Constructor
     * @param colorMapEditor1DWidget Reference to one-dimensional color map editor widget
     */
    ColorMapEditor1DScene(ColorMapEditor1DWidget& colorMapEditor1DWidget);

protected:

    /**
     * Invoked when a mouse button is pressed
     * @param graphicsSceneMouseEvent Pointer to graphics scene mouse event
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* graphicsSceneMouseEvent);

private:
    ColorMapEditor1DWidget&     _colorMapEditor1DWidget;        /** Reference to owning one-dimensional color map edit widget */

signals:
    void pressed(QGraphicsSceneMouseEvent* graphicsSceneMouseEvent);
};

}
}
