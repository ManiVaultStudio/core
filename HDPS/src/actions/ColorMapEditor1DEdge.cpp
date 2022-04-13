#include "ColorMapEditor1DEdge.h"
#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DWidget.h"

#include <QGraphicsRectItem>
#include <QPainter>
#include <QEvent>
#include <QStyleOption>
#include <QDebug>

#define COLOR_MAP_EDITOR_1D_EDGE_VERBOSE

namespace hdps {

namespace gui {

ColorMapEditor1DEdge::ColorMapEditor1DEdge(ColorMapEditor1DWidget& colorMapEditor1DWidget, ColorMapEditor1DNode* sourceNode, ColorMapEditor1DNode* targetNode) :
    QObject(&colorMapEditor1DWidget),
    QGraphicsItem(),
    _colorMapEditor1DWidget(colorMapEditor1DWidget),
    _sourceNode(sourceNode),
    _targetNode(targetNode)
{
    Q_ASSERT(sourceNode != nullptr);
    Q_ASSERT(targetNode != nullptr);

    setAcceptedMouseButtons(Qt::NoButton);

    _sourceNode->addEdge(this);
    _targetNode->addEdge(this);

    _colorMapEditor1DWidget.installEventFilter(this);
}

ColorMapEditor1DNode* ColorMapEditor1DEdge::getSourceNode() const
{
    return _sourceNode;
}

ColorMapEditor1DNode* ColorMapEditor1DEdge::getTargetNode() const
{
    return _targetNode;
}

bool ColorMapEditor1DEdge::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::EnabledChange:
        {
            if (target == &_colorMapEditor1DWidget)
                update();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

QLineF ColorMapEditor1DEdge::getLine() const
{
    if (!_sourceNode || !_targetNode)
        return QLineF();

    return QLineF(_sourceNode->pos(), _targetNode->pos());
}

QRectF ColorMapEditor1DEdge::boundingRect() const
{
    const auto line = getLine();

    return QRectF(QPointF(std::min(line.x1(), line.x2()), std::min(line.y1(), line.y2())), QPointF(std::max(line.x1(), line.x2()), std::max(line.y1(), line.y2())));
}

void ColorMapEditor1DEdge::paint(QPainter *painter, const QStyleOptionGraphicsItem*, QWidget*)
{
#ifdef COLOR_MAP_EDITOR_1D_EDGE_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto line = getLine();

    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    QStyleOption styleOption;

    styleOption.init(&_colorMapEditor1DWidget);

    QPen perimeterPen;

    perimeterPen.setWidth(2);
    perimeterPen.setColor(_colorMapEditor1DWidget.isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText));

    painter->setPen(perimeterPen);
    painter->drawLine(line);
}

}
}
