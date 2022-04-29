#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DWidget.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace hdps {

namespace gui {

ColorMapEditor1DScene::ColorMapEditor1DScene(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QGraphicsScene(&colorMapEditor1DWidget),
    _colorMapEditor1DWidget(colorMapEditor1DWidget)
{
    setBackgroundBrush(QColor(150, 150, 150));
}

void ColorMapEditor1DScene::mousePressEvent(QGraphicsSceneMouseEvent* graphicsSceneMouseEvent) {
    const auto scenePosition = graphicsSceneMouseEvent->scenePos();

    qDebug() << items(scenePosition);

    if ((graphicsSceneMouseEvent->buttons() & Qt::LeftButton) && items(scenePosition).count()) {
        QGraphicsScene::mousePressEvent(graphicsSceneMouseEvent);
    }
    else {
        graphicsSceneMouseEvent->accept();

        if (_colorMapEditor1DWidget.getGraphRectangle().contains(scenePosition))
            _colorMapEditor1DWidget.addNodeAtScenePosition(scenePosition);
    }
}

}
}
