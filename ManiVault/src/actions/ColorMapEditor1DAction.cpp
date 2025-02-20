// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QLinearGradient>

#ifdef _DEBUG
    #define COLOR_MAP_EDITOR_1D_ACTION
#endif

namespace mv::gui {

ColorMapEditor1DAction::ColorMapEditor1DAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _colorMapAction(*static_cast<ColorMapAction*>(parent)),
    _nodes(),
    _nodeAction(this, "Node 1D"),
    _colorMapImage(colorMapImageSize, QImage::Format::Format_ARGB32_Premultiplied)
{
    setText("1D custom color map");
    setIconByName("chart-line");

    addNode(QPointF(0.0f, 0.0f), Qt::black);
    addNode(QPointF(1.0f, 1.0f), Qt::white);

    connect(&_colorMapAction.getCustomColorMapAction(), &ColorMapEditor1DAction::toggled, this, &ColorMapEditor1DAction::updateColorMap);
}

QImage ColorMapEditor1DAction::getColorMapImage() const
{
    return _colorMapImage;
}

QVector<ColorMapEditor1DNode*>& ColorMapEditor1DAction::getNodes()
{
    return _nodes;
}

ColorMapEditor1DNode* ColorMapEditor1DAction::addNode(const QPointF& normalizedCoordinate, const QColor& color /*= Qt::gray*/)
{
    auto node = new ColorMapEditor1DNode(*this, normalizedCoordinate, color);

    connect(node, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, &ColorMapEditor1DAction::updateColorMap);
    connect(node, &ColorMapEditor1DNode::colorChanged, this, &ColorMapEditor1DAction::updateColorMap);

    _nodes << node;

    sortNodes();

    emit nodeAdded(node);

    return node;
}

void ColorMapEditor1DAction::removeNode(ColorMapEditor1DNode* node)
{
    Q_ASSERT(node != nullptr);

    if (!_nodes.contains(node))
        return;

    _nodes.removeOne(node);

    sortNodes();

    const auto index = node->getIndex();

    emit nodeAboutToBeRemoved(node);

    delete node;

    emit nodeRemoved(index);
}

ColorMapEditor1DNode* ColorMapEditor1DAction::getPreviousNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(node);

    if (indexOfSelectedNode >= 1)
        return _nodes[indexOfSelectedNode - 1];

    return _nodes.first();
}

ColorMapEditor1DNode* ColorMapEditor1DAction::getNextNode(ColorMapEditor1DNode* node) const
{
    const auto indexOfSelectedNode = _nodes.indexOf(node);

    if (indexOfSelectedNode < (_nodes.count() - 1))
        return _nodes[indexOfSelectedNode + 1];

    return _nodes.last();
}

const mv::gui::ColorMapEditor1DAction::Histogram& ColorMapEditor1DAction::getHistogram() const
{
    return _histogram;
}

void ColorMapEditor1DAction::setHistogram(const Histogram& histogram)
{
    if (histogram == _histogram)
        return;

    _histogram = histogram;

    emit histogramChanged(_histogram);
}

void ColorMapEditor1DAction::sortNodes()
{
    std::sort(_nodes.begin(), _nodes.end(), [](auto nodeA, auto nodeB) -> bool {
        return nodeA->getNormalizedCoordinate().x() < nodeB->getNormalizedCoordinate().x();
    });

    for (auto node : _nodes)
        node->setIndex(_nodes.indexOf(node));
}

void ColorMapEditor1DAction::updateColorMap()
{
#ifdef COLOR_MAP_EDITOR_1D_ACTION
    qDebug() << __FUNCTION__;
#endif

    QLinearGradient colorGradient(0, 0, colorMapImageSize.width(), colorMapImageSize.height()), alphaGradient(0, 0, colorMapImageSize.width(), colorMapImageSize.height());

    for (auto node : _nodes) {
        auto color = node->getColor();

        colorGradient.setColorAt(node->getNormalizedCoordinate().x(), color);
        alphaGradient.setColorAt(node->getNormalizedCoordinate().x(), QColor(node->getNormalizedCoordinate().y() * 255.0f, 0, 0, 255));
    }

    _colorMapImage = QImage(colorMapImageSize, QImage::Format::Format_ARGB32_Premultiplied);

    const auto colorMaprectangle = QRect(QPoint(), colorMapImageSize);

    auto colorImage = QImage(colorMapImageSize, QImage::Format::Format_ARGB32);
    auto alphaImage = QImage(colorMapImageSize, QImage::Format::Format_ARGB32);

    QPainter colorPainter(&colorImage), alphaPainter(&alphaImage);

    colorPainter.fillRect(colorMaprectangle, colorGradient);
    alphaPainter.fillRect(colorMaprectangle, alphaGradient);

    for (int pixelX = 0; pixelX < colorMapImageSize.width(); pixelX++) {
        const auto pixelCoordinate = QPoint(pixelX, 0);

        auto pixelColor = colorImage.pixelColor(pixelCoordinate);
        pixelColor.setAlpha(alphaImage.pixelColor(pixelCoordinate).red());

        _colorMapImage.setPixelColor(pixelCoordinate, pixelColor);
    }

    //painter.eraseRect(colorMaprectangle);
    //painter.fillRect(colorMaprectangle, QColor(0, 0, 0, 0));
    //painter.fillRect(colorMaprectangle, QColor(255, 0, 0, 0));
    //painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    //painter.setBrush(Qt::NoBrush);
    //painter.fillRect(colorMaprectangle, colorGradient);

    emit _colorMapAction.imageChanged(_colorMapImage);
}

void ColorMapEditor1DAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicColorMapEditor1DAction = dynamic_cast<ColorMapEditor1DAction*>(publicAction);

    Q_ASSERT(publicColorMapEditor1DAction != nullptr);

    if (publicColorMapEditor1DAction == nullptr)
        return;

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void ColorMapEditor1DAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    WidgetAction::disconnectFromPublicAction(recursive);
}

void ColorMapEditor1DAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    removeNode(_nodes.last());
    removeNode(_nodes.first());

    auto nodes = variantMap["Nodes"].toList();

    for (auto node : nodes) {
        const auto nodeMap = node.toMap();
        addNode(QPointF(nodeMap["NormX"].toFloat(), nodeMap["NormY"].toFloat()), QColor(nodeMap["ColorR"].toInt(), nodeMap["ColorG"].toInt(), nodeMap["ColorB"].toInt()));
    }
}

QVariantMap ColorMapEditor1DAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    QVariantList nodes;

    for (auto node : _nodes)
        nodes << node->toVariantMap();

    variantMap["Nodes"] = nodes;

    return variantMap;
}

ColorMapEditor1DAction::Widget::Widget(QWidget* parent, ColorMapEditor1DAction* colorMapEditor1DAction) :
    WidgetActionWidget(parent, colorMapEditor1DAction),
    _colorMapEditor1DWidget(this, *colorMapEditor1DAction),
    _goToFirstNodeAction(this, "First node"),
    _goToPreviousNodeAction(this, "Previous node"),
    _goToNextNodeAction(this, "Next node"),
    _goToLastNodeAction(this, "Last node"),
    _removeNodeAction(this, "Remove node")
{
    setAutoFillBackground(true);
    setFixedWidth(300);

    _goToFirstNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToPreviousNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToNextNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _goToLastNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _removeNodeAction.setDefaultWidgetFlags(TriggerAction::Icon);

    _goToFirstNodeAction.setToolTip("Go to the first node");
    _goToPreviousNodeAction.setToolTip("Go to the previous node");
    _goToNextNodeAction.setToolTip("Go to the next node");
    _goToLastNodeAction.setToolTip("Go to the last node");
    _removeNodeAction.setToolTip("Remove the selected node");

    _goToFirstNodeAction.setIconByName("angle-double-left");
    _goToPreviousNodeAction.setIconByName("angle-left");
    _goToNextNodeAction.setIconByName("angle-right");
    _goToLastNodeAction.setIconByName("angle-double-right");
    _removeNodeAction.setIconByName("trash");

    auto layout         = new QVBoxLayout();
    auto toolbarLayout  = new QHBoxLayout();

    toolbarLayout->addStretch(1);
    toolbarLayout->addWidget(_goToFirstNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToPreviousNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToNextNodeAction.createWidget(this));
    toolbarLayout->addWidget(_goToLastNodeAction.createWidget(this));
    toolbarLayout->addWidget(_removeNodeAction.createWidget(this));

    //layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);
    layout->addWidget(&_colorMapEditor1DWidget);
    layout->addLayout(toolbarLayout);
    layout->addWidget(colorMapEditor1DAction->getNodeAction().createWidget(this));

    setLayout(layout);

    connect(&_goToFirstNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNodes().first());
    });

    connect(&_goToPreviousNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        _colorMapEditor1DWidget.selectNode(colorMapEditor1DAction->getPreviousNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToNextNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        _colorMapEditor1DWidget.selectNode(colorMapEditor1DAction->getNextNode(_colorMapEditor1DWidget.getCurrentNode()));
    });

    connect(&_goToLastNodeAction, &TriggerAction::triggered, this, [this] {
        _colorMapEditor1DWidget.selectNode(_colorMapEditor1DWidget.getNodes().last());
    });

    connect(&_removeNodeAction, &TriggerAction::triggered, this, [this, colorMapEditor1DAction] {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = _colorMapEditor1DWidget.getNodes();

        if (currentNode == nodes.first() || currentNode == nodes.last())
            return;

        colorMapEditor1DAction->removeNode(currentNode);
    });

    const auto updateActions = [this, colorMapEditor1DAction]() -> void {
        const auto currentNode  = _colorMapEditor1DWidget.getCurrentNode();
        const auto nodes        = colorMapEditor1DAction->getNodes();

        if (currentNode) {
            _goToFirstNodeAction.setEnabled(currentNode && currentNode != nodes.first());
            _goToPreviousNodeAction.setEnabled(currentNode && colorMapEditor1DAction->getPreviousNode(currentNode) != currentNode);
            _goToNextNodeAction.setEnabled(currentNode && colorMapEditor1DAction->getNextNode(currentNode) != currentNode);
            _goToLastNodeAction.setEnabled(currentNode && currentNode != nodes.last());
            _removeNodeAction.setEnabled(currentNode && currentNode != nodes.first() && currentNode != nodes.last());
        }
        else {
            _goToFirstNodeAction.setEnabled(false);
            _goToPreviousNodeAction.setEnabled(false);
            _goToNextNodeAction.setEnabled(false);
            _goToLastNodeAction.setEnabled(false);
            _removeNodeAction.setEnabled(false);
        }
    };

    connect(&_colorMapEditor1DWidget, &ColorMapEditor1DWidget::currentNodeChanged, this, updateActions);

    updateActions();
}

}
