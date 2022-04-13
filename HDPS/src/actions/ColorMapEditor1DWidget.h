#pragma once

#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DNode.h"

#include <QWidget>
#include <QResizeEvent>
#include <QGraphicsView>

namespace hdps {

namespace gui {

class ColorMapEditor1DAction;

class ColorMapEditor1DWidget : public QGraphicsView
{
    Q_OBJECT

public:

    ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction);

    /**Function used to add a node at a given position. */
    void addNode(QPointF position);

    /** Function used to remove a node at a given position. */
    void removeNode(ColorMapEditor1DNode* node);

    /** Function used to redraw the lines between the nodes. */
    void redrawEdges();

    /** Function used to draw the axes in the transferfunction. */
    void drawBackground(QPainter* painter, const QRectF& rect);

    /** Function used to recreate the colormap. */
    void createColorMap();

    /** Get the color map. */
    QImage getColorMap() {
        return _colorMap;
    }

    void selectNode(ColorMapEditor1DNode* node);

public:

    /**
     * Get nodes
     * @return Nodes
     */
    QVector<ColorMapEditor1DNode*> getNodes();

    /**
     * Get previous node
     * @param node Pointer to node of which to obtain the previous node
     * @return Pointer to previous node (node if it has no previous node)
     */
    ColorMapEditor1DNode* getPreviousNode(ColorMapEditor1DNode* node) const;

    /**
     * Get next node
     * @param node Pointer to node of which to obtain the next node
     * @return Pointer to next node (node if it has no previous node)
     */
    ColorMapEditor1DNode* getNextNode(ColorMapEditor1DNode* node) const;

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
    ColorMapEditor1DAction&         _colorMapEditor1DAction;
    QCursor                         _cursor;
    ColorMapEditor1DScene           _scene;
    std::uint32_t                   _margin;
    QRectF                          _graphRectangle;        /** Nodes graph rectangle */
    QVector<ColorMapEditor1DNode*>  _nodes;                 /** All sorted nodes */
    QVector<QGraphicsItem*>         _edgeList;
    ColorMapEditor1DNode*           _currentNode;
    QImage                          _colorMap;

signals:

    void colorMapChanged(const QImage& colorMap); // Signal when the colormap has been altered.
    void valueChanged(const QPointF& currentNodePosition);

    /**
     * Signals that the current node changed
     * @param currentNode Pointer to the current node
     */
    void currentNodeChanged(ColorMapEditor1DNode* currentNode);
    
};

}
}
