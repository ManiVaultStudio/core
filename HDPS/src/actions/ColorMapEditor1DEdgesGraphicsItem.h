// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QGraphicsItem>

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;
class ColorMapEditor1DNode;

/**
 * Color map editor 1D edges graphics item class
 *
 * Graphics item class which visually connected nodes in a one-dimensional color map
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class ColorMapEditor1DEdgesGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    /**
     * Constructor
     * @param colorMapEditor1DWidget Reference to one-dimensional color map editor widget
     */
    ColorMapEditor1DEdgesGraphicsItem(ColorMapEditor1DWidget& colorMapEditor1DWidget);

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected:

    /**
     * Get edges bounding rectangle
     * @return Bounding rectangle
     */
    QRectF boundingRect() const override;

    /**
     * Paint the edges
     * @param painter Pointer to painter
     * @param option Pointer to style options
     * @param widget Pointer to widget
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    ColorMapEditor1DWidget&     _colorMapEditor1DWidget;    /** Reference to one-dimensional color map editor widget */
};

}
}
