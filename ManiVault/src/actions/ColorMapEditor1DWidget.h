// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DNode.h"

#include <QWidget>
#include <QResizeEvent>
#include <QGraphicsView>

namespace mv::gui {

class ColorMapEditor1DAction;
class ColorMapEditor1DNodeGraphicsItem;

/**
 * Color map editor one-dimensional widget class
 *
 * Widget class for drawing the one-dimensional color map editor
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class CORE_EXPORT ColorMapEditor1DWidget : public QGraphicsView
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param colorMapEditor1DAction Reference to one-dimensional color map editor action
     */
    ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction);

    /** Destructor */
    ~ColorMapEditor1DWidget();

    /**
     * Invoked when the widget is shown 
     * @param Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Add node at a specific scene position
     * @param scenePosition Position in scene coordinates
     */
    void addNodeAtScenePosition(const QPointF& scenePosition);

    /**
     * Add node graphics item to the scene
     * @param node Pointer to node
     */
    void addNodeGraphicsItem(ColorMapEditor1DNode* node);

    /**
     * Remove node graphics item
     * @param node pointer to node to remove
     */
    void removeNodeGraphicsItem(ColorMapEditor1DNode* node);

    /**
     * Draws the editor background elements (axes etc.)
     * @param painter Pointer to painter
     * @param rect Background rectangle
     */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

    /**
     * Get the color map
     * @return Color map image
     */
    QImage getColorMap();

    /**
     * Select a single node
     * @param node Pointer to a node
     */
    void selectNode(ColorMapEditor1DNode* node);

public:

    /**
     * Get nodes
     * @return Nodes
     */
    QVector<ColorMapEditor1DNode*>& getNodes();

    /**
     * Get nodes
     * @return Nodes
     */
    const QVector<ColorMapEditor1DNode*>& getNodes() const;

    /**
     * Get current node (if one is selected)
     * @return Pointer to current node (if any)
     */
    ColorMapEditor1DNode* getCurrentNode();

    /**
     * Get graph rectangle (in which the nodes reside)
     * @return Graph rectangle
     */
    QRectF getGraphRectangle() const;

public: // Action getters

    ColorMapEditor1DAction& getColorMapEditor1DAction() { return _colorMapEditor1DAction; }

private:
    ColorMapEditor1DAction&                         _colorMapEditor1DAction;    /** Reference to one-dimensional color map editor action */
    ColorMapEditor1DScene                           _scene;                     /** Editor graphics scene */
    QMargins                                        _margins;                   /** Graph rectangle margins */
    QRectF                                          _graphRectangle;            /** Nodes graph rectangle */
    ColorMapEditor1DNode*                           _currentNode;               /** Currently selected node */
    QVector<ColorMapEditor1DNodeGraphicsItem*>      _nodes;                     /** Nodes in the editor */
    QImage                                          _colorMap;                  /** Current color map */

signals:

    /**
     * Signals that the color map changed
     * @param colorMap Reference to color map
     */
    void colorMapChanged(const QImage& colorMap);

    /**
     * Signals that the current node changed
     * @param currentNode Pointer to the current node
     */
    void currentNodeChanged(ColorMapEditor1DNode* currentNode);
    
};

}
