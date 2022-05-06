#pragma once

#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DNode.h"

#include <QWidget>
#include <QResizeEvent>
#include <QGraphicsView>

namespace hdps {

namespace gui {

class ColorMapEditor1DAction;
class ColorMapEditor1DNodeGraphicsItem;

class ColorMapEditor1DWidget : public QGraphicsView
{
    Q_OBJECT

public:

    ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction);

    /** Destructor */
    ~ColorMapEditor1DWidget();

    /**
     * Invoked when the widget is shown 
     * @param Pointer to show event
     */
    void showEvent(QShowEvent* event);

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

    /** Function used to draw the axes in the transferfunction. */
    void drawBackground(QPainter* painter, const QRectF& rect) override;

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
    ColorMapEditor1DAction&                         _colorMapEditor1DAction;
    QCursor                                         _cursor;
    ColorMapEditor1DScene                           _scene;
    QMargins                                        _margins;
    QRectF                                          _graphRectangle;            /** Nodes graph rectangle */
    ColorMapEditor1DNode*                           _currentNode;
    QVector<ColorMapEditor1DNodeGraphicsItem*>      _nodes;
    QImage                                          _colorMap;

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
