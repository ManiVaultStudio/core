#pragma once

#include <QGraphicsItem>

namespace hdps {

namespace gui {

class ColorMapEditor1DWidget;
class ColorMapEditor1DNode;

class ColorMapEditor1DEdge : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param colorMapEditor1DWidget Reference to one-dimensional color map editor widget
     * @param sourceNode Pointer to source node
     * @param targetNode Pointer to target node
     */
    ColorMapEditor1DEdge(ColorMapEditor1DWidget& colorMapEditor1DWidget, ColorMapEditor1DNode* sourceNode, ColorMapEditor1DNode* targetNode);

    ColorMapEditor1DNode* getSourceNode() const;
    ColorMapEditor1DNode* getTargetNode() const;

    /**
     * Respond to target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

protected:

    /**
     * Get edge line
     * @return Edge line
     */
    QLineF getLine() const;

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

protected:
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    ColorMapEditor1DWidget&     _colorMapEditor1DWidget;    /** Reference to one-dimensional color map editor widget */
    ColorMapEditor1DNode*       _sourceNode;                /** Pointer to source node */
    ColorMapEditor1DNode*       _targetNode;                /** Pointer to target node */
};

}
}
