// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QGraphicsScene>

class QGraphicsSceneMouseEvent;

namespace mv::gui {

class ColorMapEditor1DWidget;

/**
 * Color map editor one-dimensional graphics scene class
 *
 * Graphics scene class for drawing the one-dimensional color map editor
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class CORE_EXPORT ColorMapEditor1DScene : public QGraphicsScene
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
