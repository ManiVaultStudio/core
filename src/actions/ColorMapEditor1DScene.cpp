// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DWidget.h"

#include "CoreInterface.h"

#include <QGraphicsSceneMouseEvent>
#include <QDebug>

namespace mv::gui {

ColorMapEditor1DScene::ColorMapEditor1DScene(ColorMapEditor1DWidget& colorMapEditor1DWidget) :
    QGraphicsScene(&colorMapEditor1DWidget),
    _colorMapEditor1DWidget(colorMapEditor1DWidget)
{
    const auto updateCustomStyle = [this]() -> void {
        setBackgroundBrush(qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Window));
    };

    updateCustomStyle();

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, updateCustomStyle);
}

void ColorMapEditor1DScene::mousePressEvent(QGraphicsSceneMouseEvent* graphicsSceneMouseEvent) {
    const auto scenePosition = graphicsSceneMouseEvent->scenePos();

    if ((graphicsSceneMouseEvent->buttons() & Qt::LeftButton)) {
        if (items(scenePosition).count() >= 1) {
            QGraphicsScene::mousePressEvent(graphicsSceneMouseEvent);
        }
        else {
            graphicsSceneMouseEvent->accept();

            if (_colorMapEditor1DWidget.getGraphRectangle().contains(scenePosition))
                _colorMapEditor1DWidget.addNodeAtScenePosition(scenePosition);
        }
    }

    if ((graphicsSceneMouseEvent->buttons() & Qt::RightButton))
        QGraphicsScene::mousePressEvent(graphicsSceneMouseEvent);
}

}
