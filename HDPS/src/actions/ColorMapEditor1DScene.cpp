#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DWidget.h"

#include <QGraphicsSceneMouseEvent>

namespace hdps {

namespace gui {

ColorMapEditor1DScene::ColorMapEditor1DScene(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QGraphicsScene(&colorMapEditor1DWidget),
    _cursor(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget)
{
}

void ColorMapEditor1DScene::mousePressEvent(QGraphicsSceneMouseEvent* graphicsSceneMouseEvent) {
    const auto position = graphicsSceneMouseEvent->scenePos();

    QGraphicsScene::mousePressEvent(graphicsSceneMouseEvent);

    /*
    if (graphicsSceneMouseEvent->buttons() & Qt::LeftButton) {
        _colorMapEditor1DWidget.findNode(position, "Left");
    }
    else if (graphicsSceneMouseEvent->buttons() & Qt::RightButton) {
        _colorMapEditor1DWidget.findNode(position, "Right");
    }
    else if (graphicsSceneMouseEvent->buttons() & Qt::MiddleButton) {
        _colorMapEditor1DWidget.findNode(position, "Middle");
    }
    */
}

}
}
