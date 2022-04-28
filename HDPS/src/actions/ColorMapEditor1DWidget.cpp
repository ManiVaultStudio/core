#include "ColorMapEditor1DWidget.h"
#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DNodeGraphicsItem.h"
#include "ColorMapEditor1DEdge.h"
#include "ColorMapEditor1DAction.h"

#include <QPainter>
#include <QMessageBox>
#include <QStyleOption>

#include <algorithm>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_WIDGET_VERBOSE
#endif

namespace hdps {

namespace gui {

ColorMapEditor1DWidget::ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction) :
    QGraphicsView(parent),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _cursor(),
    _scene(*this),
    _margins(60, 10, 10, 10),
    _graphRectangle(),
    _currentNode(),
    _nodes(),
    _colorMap()
{
    setMinimumHeight(200);
    installEventFilter(this);
}

ColorMapEditor1DWidget::~ColorMapEditor1DWidget()
{
    _colorMapEditor1DAction.getNodeAction().connectToNode(nullptr);
}

void ColorMapEditor1DWidget::showEvent(QShowEvent* event)
{
    _scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scene.setSceneRect(rect());

    _graphRectangle = rect().marginsRemoved(_margins);

    setScene(&_scene);
    //setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    connect(&_colorMapEditor1DAction, &ColorMapEditor1DAction::nodeAdded, this, &ColorMapEditor1DWidget::addNodeGraphicsItem);
    connect(&_colorMapEditor1DAction, &ColorMapEditor1DAction::nodeAboutToBeRemoved, this, &ColorMapEditor1DWidget::removeNodeGraphicsItem);

    connect(&_colorMapEditor1DAction, &ColorMapEditor1DAction::nodeRemoved, this, [this](const std::uint32_t& index) -> void {
        selectNode(_colorMapEditor1DAction.getNodes()[std::max(0u, index - 1)]);
    });

    for (auto node : _colorMapEditor1DAction.getNodes())
        addNodeGraphicsItem(node);
}

bool ColorMapEditor1DWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::EnabledChange:
            repaint();
            break;

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
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
    _currentNode = node;

    emit currentNodeChanged(_currentNode);

    _colorMapEditor1DAction.getNodeAction().connectToNode(_currentNode);
}

QVector<ColorMapEditor1DNode*>& ColorMapEditor1DWidget::getNodes()
{
    return _colorMapEditor1DAction.getNodes();
}

const QVector<ColorMapEditor1DNode*>& ColorMapEditor1DWidget::getNodes() const
{
    return _colorMapEditor1DAction.getNodes();
}

ColorMapEditor1DNode* ColorMapEditor1DWidget::getCurrentNode()
{
    return _currentNode;
}

QRectF ColorMapEditor1DWidget::getGraphRectangle() const
{
    return _graphRectangle;
}

void ColorMapEditor1DWidget::addNodeAtScenePosition(const QPointF& scenePosition) {

#ifdef COLOR_MAP_EDITOR_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto normalizedCoordinate = QPointF((scenePosition.x() - _graphRectangle.x()) / _graphRectangle.width(), 1.0f - (scenePosition.y() - _graphRectangle.y()) / _graphRectangle.height());
    const auto node                 = _colorMapEditor1DAction.addNode(normalizedCoordinate);

    selectNode(node);
}

void ColorMapEditor1DWidget::addNodeGraphicsItem(ColorMapEditor1DNode* node)
{
#ifdef COLOR_MAP_EDITOR_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Q_ASSERT(node != nullptr);

    const auto nodeGraphicsItem = new ColorMapEditor1DNodeGraphicsItem(*this, *node);

    _nodes << nodeGraphicsItem;

    _scene.addItem(nodeGraphicsItem);
}

void ColorMapEditor1DWidget::removeNodeGraphicsItem(ColorMapEditor1DNode* node)
{
#ifdef COLOR_MAP_EDITOR_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Q_ASSERT(node != nullptr);

    auto it = std::find_if(_nodes.begin(), _nodes.end(), [node](auto nodeGraphicsItem) -> bool {
        return &nodeGraphicsItem->getNode() == node;
    });

    if (it != _nodes.end())
        _scene.removeItem(*it);
}

void ColorMapEditor1DWidget::drawBackground(QPainter* painter,const QRectF& rect)
{
    Q_UNUSED(rect);

    QStyleOption styleOption;

    styleOption.init(this);

    QPen pen;

    pen.setWidth(2);
    pen.setColor(isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

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
