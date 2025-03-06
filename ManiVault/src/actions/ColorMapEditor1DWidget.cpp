// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DWidget.h"
#include "ColorMapEditor1DScene.h"
#include "ColorMapEditor1DNodeGraphicsItem.h"
#include "ColorMapEditor1DEdgesGraphicsItem.h"
#include "ColorMapEditor1DHistogramGraphicsItem.h"
#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"

#include <QPainter>
#include <QStyleOption>

#include <algorithm>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_WIDGET_VERBOSE
#endif

namespace mv::gui {

ColorMapEditor1DWidget::ColorMapEditor1DWidget(QWidget* parent, ColorMapEditor1DAction& colorMapEditor1DAction) :
    QGraphicsView(parent),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _scene(*this),
    _margins(52, 15, 25, 40),
    _currentNode()
{
    setMinimumHeight(100);

    installEventFilter(this);

    _scene.addItem(new ColorMapEditor1DEdgesGraphicsItem(*this));
    _scene.addItem(new ColorMapEditor1DHistogramGraphicsItem(*this));

    connect(&mv::theme(), &AbstractThemeManager::colorSchemeChanged, this, [this]() -> void {
        update();
    });
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
    setBackgroundBrush(QBrush(Qt::red, Qt::SolidPattern));
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

    connect(&_colorMapEditor1DAction.getColorMapAction().getRangeAction(ColorMapAction::Axis::X), &DecimalRangeAction::rangeChanged, this, [this]() -> void {
        _scene.update();
    });
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

void ColorMapEditor1DWidget::addNodeAtScenePosition(const QPointF& scenePosition)
{
#ifdef COLOR_MAP_EDITOR_1D_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto normalizedCoordinate = QPointF((scenePosition.x() - _graphRectangle.x()) / _graphRectangle.width(), 1.0f - (scenePosition.y() - _graphRectangle.y()) / _graphRectangle.height());
    const auto node                 = _colorMapEditor1DAction.addNode(normalizedCoordinate);

    selectNode(node);
}

void ColorMapEditor1DWidget::addNodeGraphicsItem(ColorMapEditor1DNode* node)
{
#ifdef COLOR_MAP_EDITOR_1D_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    Q_ASSERT(node != nullptr);

    const auto nodeGraphicsItem = new ColorMapEditor1DNodeGraphicsItem(*this, *node);

    _nodes << nodeGraphicsItem;

    _scene.addItem(nodeGraphicsItem);
}

void ColorMapEditor1DWidget::removeNodeGraphicsItem(ColorMapEditor1DNode* node)
{
#ifdef COLOR_MAP_EDITOR_1D_WIDGET_VERBOSE
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
    QStyleOption styleOption;

    styleOption.initFrom(this);

    painter->fillRect(rect, styleOption.palette.color(isEnabled() ? QPalette::ColorGroup::Normal : QPalette::ColorGroup::Disabled, QPalette::Window));

    const auto penColor = isEnabled() ? styleOption.palette.color(QPalette::Normal, QPalette::ButtonText) : styleOption.palette.color(QPalette::Disabled, QPalette::ButtonText);

    QPen pen;

    pen.setWidthF(1.5f);
    pen.setColor(penColor);

    painter->setPen(pen);

    painter->drawRect(_graphRectangle);

    QPen labelPen, axisPen, dashedAxisPen;

    labelPen.setColor(penColor);
    axisPen.setColor(penColor);
    dashedAxisPen.setColor(penColor);

    painter->setFont(QFont("Arial", 7));

    axisPen.setWidthF(1.0f);
    dashedAxisPen.setWidthF(1.0f);

    QVector<qreal> dashes;

    dashes << 3 << 3;

    dashedAxisPen.setDashPattern(dashes);

    const auto drawHorizontalAxis = [this, painter, labelPen, axisPen, dashedAxisPen](const float& percentage) -> void {
        auto origin = _graphRectangle.bottomLeft();

        origin.setY(origin.y() - (percentage * _graphRectangle.height()));

        const auto textRectangleSize    = QSizeF(50, 20);
        const auto textRectangle        = QRectF(origin - QPointF(textRectangleSize.width() + 10, textRectangleSize.height() / 2), textRectangleSize);
        const auto startOrEndAxis       = percentage == 0.0f || percentage == 1.0f;

        painter->setPen(labelPen);
        painter->drawText(textRectangle, QString("%1%").arg(percentage * 100.0f), QTextOption(Qt::AlignVCenter | Qt::AlignRight));

        painter->setPen(axisPen);
        painter->drawLine(QPointF(textRectangle.right() + 3, textRectangle.center().y()), QPointF(_graphRectangle.left(), textRectangle.center().y()));

        if (!startOrEndAxis) {
            painter->setPen(dashedAxisPen);
            painter->drawLine(QPointF(_graphRectangle.left() + 3, textRectangle.center().y()), QPointF(_graphRectangle.right(), textRectangle.center().y()));
        }
    };

    for (float axisPercentage = 0.0f; axisPercentage <= 1.0f; axisPercentage += 0.2f)
        drawHorizontalAxis(axisPercentage);

    const auto drawVerticalAxis = [this, painter, labelPen, axisPen](const float& percentage, const float& value) -> void {
        auto origin = _graphRectangle.bottomLeft();

        origin.setX(origin.x() + (percentage * _graphRectangle.width()));

        const auto textRectangleSize    = QSizeF(100, 50);
        const auto textRectangle        = QRectF(origin - QPointF(textRectangleSize.width() / 2, -10), textRectangleSize);

        painter->setPen(labelPen);
        painter->drawText(textRectangle, QString::number(value, 'f', 2), QTextOption(Qt::AlignHCenter | Qt::AlignTop));

        painter->setPen(axisPen);
        painter->drawLine(QPointF(textRectangle.center().x(), textRectangle.top() - 3), QPointF(textRectangle.center().x(), _graphRectangle.bottom()));
    };

    auto& rangeAction = _colorMapEditor1DAction.getColorMapAction().getRangeAction(ColorMapAction::Axis::X);

    drawVerticalAxis(0.0f, rangeAction.getMinimum());
    drawVerticalAxis(1.0f, rangeAction.getMaximum());

    painter->drawText(_graphRectangle.translated(QPoint(0, _graphRectangle.height() + 18)), "Value", QTextOption(Qt::AlignHCenter | Qt::AlignTop));

    painter->translate(QPoint(18, _graphRectangle.center().y()));
    painter->rotate(-90);
    painter->drawText(QRectF(QPointF(-200, -20), QPointF(200, 0)), "Opacity", QTextOption(Qt::AlignHCenter | Qt::AlignBottom));
}

QImage ColorMapEditor1DWidget::getColorMap()
{
    return _colorMap;
}

}
