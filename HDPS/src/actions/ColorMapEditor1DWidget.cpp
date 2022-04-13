#include "ColorMapEditor1DWidget.h"
#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DEdge.h"
#include "ColorMapEditor1DAction.h"

#include <QPainter.h>
#include <QMessageBox>

#include <algorithm>

namespace hdps {

namespace gui {

ColorMapEditor1DWidget::ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction) :
    QGraphicsView(parent),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _cursor(),
    _scene(*this),
    _margin(10),
    _graphRectangle(),
    _nodes(QVector<ColorMapEditor1DNode*> (2)),
    _edgeList(QVector<QGraphicsItem*> (1)),
    _currentNode(),
    _colorMap()
{
    setMinimumHeight(250);

    _scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    //_scene.setSceneRect(rect());

    _graphRectangle = rect().marginsRemoved(QMargins(_margin, _margin, _margin, _margin));

    setScene(&_scene);
    setCacheMode(CacheBackground);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    auto node1 = new ColorMapEditor1DNode(*this);
    auto node2 = new ColorMapEditor1DNode(*this);

    _nodes[0] = node1;
    _nodes[1] = node2;

    _scene.addItem(_nodes[0]);
    _scene.addItem(_nodes[1]);    

    auto edge1 = new ColorMapEditor1DEdge(*this, node1, node2);

    _edgeList[0] = edge1;

    _scene.addItem(_edgeList[0]);

    node1->setPos(_graphRectangle.bottomLeft());
    node2->setPos(_graphRectangle.topRight());
}

// This function is used to gather all data created by this widget and cummulates this into a QImage with a height of 1 and a width of 256, this QImage is the created as a colormap.
void ColorMapEditor1DWidget::createColorMap()
{
    /*
    // Initialize the colormap.
    const auto noSteps = 256;
    QImage colorMap(noSteps, 1, QImage::Format::Format_ARGB32);

    // Sort the nodes in the x-value order.
    auto sortedNodeList = sortNodes();

    // Translate the windowsize to the size of the colormap.
    float stepsize = (float)noSteps /(float)width();
    float alphaStepSize = (float)1/(float)height();    
    int currentPosition = 0;
    int nextPosition = 0;

    // Walk over the nodeList in order to construct the colormap.
    for (int i = 0; i < _nodeList.size()-1; i++) {
        // Get the index of the current and the next node, orderd by x-value.
        int currentNodeIndex = sortedNodeList[i].second;
        int nextNodeIndex = sortedNodeList[i + 1].second;

        // Get the current and next node color information.
        auto currentColor = _nodeColorList[currentNodeIndex];
        auto nextColor = _nodeColorList[nextNodeIndex];
        auto currentAlpha = abs(_nodeList[currentNodeIndex]->y() - height()) * alphaStepSize;
        auto nextAlpha = abs(_nodeList[nextNodeIndex]->y() - height()) * alphaStepSize;
        currentPosition = sortedNodeList[i].first * stepsize;
        nextPosition = sortedNodeList[i + 1].first * stepsize;

        // Iterate over the pixelvalues inside the colormap to determine the values of the colors between the current and next nodes.
        for (int i = currentPosition; i < nextPosition; i++) {
            // Calculate linear interpolation between the 2 nodes of color and alpha of the current pixelvalue.
            double ratio = (double)(i - currentPosition) / (double)(nextPosition - currentPosition);
            double resultRed = currentColor.redF() + ratio * (nextColor.redF() - currentColor.redF());
            double resultGreen = currentColor.greenF() + ratio * (nextColor.greenF() - currentColor.greenF());
            double resultBlue = currentColor.blueF() + ratio * (nextColor.blueF() - currentColor.blueF());
            double alphaResult = currentAlpha + ratio * (nextAlpha - currentAlpha);

            // Create the found color and add to the colormap.
            QColor interpolatedColor;
            interpolatedColor.setBlueF(resultBlue);
            interpolatedColor.setRedF(resultRed);
            interpolatedColor.setGreenF(resultGreen);
            interpolatedColor.setAlphaF(alphaResult);
            colorMap.setPixelColor(i, 0, interpolatedColor);
        }
    }

    // Save the colormap.
    _colorMap = colorMap;

    // Emit a signal that the colormap has been changed.
    emit colorMapChanged(_colorMap);
    */
}

void ColorMapEditor1DWidget::selectNode(ColorMapEditor1DNode* node)
{
    if (node == _currentNode)
        return;

    _currentNode = node;

    emit currentNodeChanged(_currentNode);

    _colorMapEditor1DAction.getNodeAction().connectToNode(_currentNode);
}

QVector<ColorMapEditor1DNode*> ColorMapEditor1DWidget::getNodes()
{
    return _nodes;
}

ColorMapEditor1DNode* ColorMapEditor1DWidget::getPreviousNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(_currentNode);

    if (indexOfSelectedNode >= 1)
        return _nodes[indexOfSelectedNode - 1];

    return _nodes.first();
}

ColorMapEditor1DNode* ColorMapEditor1DWidget::getNextNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(_currentNode);

    if (indexOfSelectedNode < (_nodes.count() - 1))
        return _nodes[indexOfSelectedNode + 1];

    return _nodes.last();
}

ColorMapEditor1DNode* ColorMapEditor1DWidget::getCurrentNode()
{
    return _currentNode;
}

QRectF ColorMapEditor1DWidget::getGraphRectangle() const
{
    return _graphRectangle;
}

void ColorMapEditor1DWidget::removeNode(ColorMapEditor1DNode* node)
{
    Q_ASSERT(node != nullptr);

    _scene.removeItem(node);
    _nodes.removeOne(node);

    redrawEdges();
}

// This function is used to add a node at a given position.
void ColorMapEditor1DWidget::addNode(QPointF position) {
    // Create a new node at the stated position and create data for this node.
    auto newNode = new ColorMapEditor1DNode(*this);

    _scene.addItem(newNode);

    newNode->setPos(position);
    _nodes.push_back(newNode);
    _currentNode = newNode;

    std::sort(_nodes.begin(), _nodes.end(), [](auto nodeA, auto nodeB) -> bool {
        return nodeA->x() < nodeB->x();
    });

    for (auto node : _nodes)
        node->setIndex(_nodes.indexOf(node));

    _colorMapEditor1DAction.getNodeAction().changeNodeColor(Qt::yellow);
    redrawEdges();
    createColorMap();
}

// This function redraws the lines between the nodes that indicate the transferfunction.
void ColorMapEditor1DWidget::redrawEdges() {
    // Remove all edges in the list.
    for (int i = 0; i < _edgeList.size(); i++) {
        _scene.removeItem(_edgeList[i]);
    }
    _edgeList.clear();

    /*
    // Add lines between all nodes.    
    for (int i = 0; i < sortedIndices.size() - 1; i++) {
        auto newEdge = new ColorMapEditor1DEdge(_nodeList[sortedIndices[i].second], _nodeList[sortedIndices[i + 1].second]);
        _edgeList.push_back(newEdge);
        _scene.addItem(_edgeList[i]);
    }
    */

    // Recreate colormap after the edges and nodes have been changed.
    createColorMap();
}

// This function draws the axis inside the transferfunction.
void ColorMapEditor1DWidget::drawBackground(QPainter* painter,const QRectF& rect)
{
    Q_UNUSED(rect);

    QPen pen;
    pen.setWidth(2);
    painter->setPen(pen);

    painter->drawRect(_graphRectangle);

    /*
    pen.setWidth(1);
    painter->setPen(pen);

    //QRectF colorMapRect(QPoint(0, height()+50), QPoint(width(), height() + 45));
    //painter->drawRect(colorMapRect);

    // Divide the windowheight in 4.
    auto lineHeightWidth = (float)height() / 4;

    painter->drawText(QPoint(-30, 0), "100%");
    painter->drawText(QPoint(-30, lineHeightWidth), "75%");
    painter->drawText(QPoint(-30, lineHeightWidth * 2), "50%");
    painter->drawText(QPoint(-30, lineHeightWidth * 3), "25%");
    painter->drawText(QPoint(-30, lineHeightWidth * 4), "0%");

    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::lightGray);

    QVector<qreal> dashes;
    qreal space = 5;
    dashes << 5 << space  << 5 << space ;

    pen.setDashPattern(dashes);
    painter->setPen(pen);

    // Draw dashed lines to indicate color alpha for the transferfunction.
    QLine dashedLine75(QPoint(0,lineHeightWidth), QPoint(width(),lineHeightWidth));
    painter->drawLine(dashedLine75);

    QLine dashedLine50(QPoint(0, lineHeightWidth*2), QPoint(width(), lineHeightWidth*2));
    painter->drawLine(dashedLine50);

    QLine dashedLine25(QPoint(0, lineHeightWidth*3), QPoint(width(), lineHeightWidth*3));
    painter->drawLine(dashedLine25);

    //QPixmap pixelMap;
    //pixelMap.fromImage(_colorMap);
    //painter->drawPixmap(colorMapRect.topLeft(), pixelMap);

    */
}

}
}
