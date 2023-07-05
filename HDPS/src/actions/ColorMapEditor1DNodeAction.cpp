// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMapEditor1DNodeAction.h"
#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <QGridLayout>
#include <QSpinBox>

namespace hdps {

namespace gui {

ColorMapEditor1DNodeAction::ColorMapEditor1DNodeAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _colorMapEditor1DAction(*static_cast<ColorMapEditor1DAction*>(parent)),
    _currentNode(nullptr),
    _colorAction(this, "Color", Qt::yellow),
    _opacityAction(this, "Intensity", 0.0f, 100.0f, 0.0f, 0),
    _valueAction(this, "Value", 0.0f, 100.0f, 0.0f, 2)
{
    _colorAction.setEnabled(false);
    _opacityAction.setEnabled(false);
    _valueAction.setEnabled(false);

    _opacityAction.setSuffix("%");

    connect(&_colorAction, &ColorAction::colorChanged, this, [this](const QColor& color) {
        if (!_currentNode)
            return;

        _currentNode->setColor(color);
    });

    const auto updateNodeNormalizedCoordinate = [this]() -> void {
        if (!_currentNode)
            return;

        auto& rangeAction = _colorMapEditor1DAction.getColorMapAction().getRangeAction(ColorMapAction::Axis::X);

        const auto minimum      = rangeAction.getMinimum();
        const auto maximum      = rangeAction.getMaximum();
        const auto length       = maximum - minimum;
        const auto x            = (_valueAction.getValue() - minimum) / length;
        const auto y            = 0.01f * _opacityAction.getValue();

        _currentNode->setNormalizedCoordinate(QPointF(x, y));
    };

    connect(&_valueAction, &DecimalAction::valueChanged, this, updateNodeNormalizedCoordinate);
    connect(&_opacityAction, &DecimalAction::valueChanged, this, updateNodeNormalizedCoordinate);

    connect(&_colorMapEditor1DAction, &ColorMapEditor1DAction::nodeAboutToBeRemoved, this, [this](ColorMapEditor1DNode* node) -> void {
        if (node != _currentNode)
            return;

        disconnectFromNode(node);
    });

    connect(&_colorMapEditor1DAction.getColorMapAction().getRangeAction(ColorMapAction::Axis::X), &DecimalRangeAction::rangeChanged, this, [this]() -> void {
        nodeChanged();
    });
}

void ColorMapEditor1DNodeAction::connectToNode(ColorMapEditor1DNode* node)
{
    if (_currentNode)
        disconnectFromNode(_currentNode);

    _currentNode = node;

    const auto enable = _currentNode != nullptr;

    _colorAction.setEnabled(enable);
    _opacityAction.setEnabled(enable);

    if (_currentNode == nullptr)
        return;

    const auto& nodes = _colorMapEditor1DAction.getNodes();

    _valueAction.setEnabled(_currentNode != nodes.first() && _currentNode != nodes.last());

    connect(_currentNode, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, &ColorMapEditor1DNodeAction::nodeChanged);

    nodeChanged();
}

void ColorMapEditor1DNodeAction::disconnectFromNode(ColorMapEditor1DNode* node)
{
    if (node == nullptr)
        return;

    disconnect(node, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, nullptr);

    _currentNode = nullptr;
}

void ColorMapEditor1DNodeAction::nodeChanged()
{
    if (_currentNode == nullptr)
        return;

    const auto normalizedValueToRange = [this](const float& normalizedValue) -> float {
        auto& rangeAction = _colorMapEditor1DAction.getColorMapAction().getRangeAction(ColorMapAction::Axis::X);

        const auto minimum      = rangeAction.getMinimum();
        const auto maximum      = rangeAction.getMaximum();
        const auto length       = maximum - minimum;
        
        return minimum + (normalizedValue * length);
    };

    const auto normalizedCoordinate = _currentNode->getNormalizedCoordinate();
    const auto limits               = _currentNode->getLimits();

    _colorAction.setColor(_currentNode->getColor());
    _valueAction.initialize(normalizedValueToRange(limits.left()), normalizedValueToRange(limits.right()), normalizedValueToRange(normalizedCoordinate.x()), 2);
    _opacityAction.setValue(normalizedCoordinate.y() * 100.0f);
}

ColorMapEditor1DNodeAction::Widget::Widget(QWidget* parent, ColorMapEditor1DNodeAction* colorMapEditor1DNodeAction) :
    WidgetActionWidget(parent, colorMapEditor1DNodeAction)
{
    auto layout = new QGridLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);

    layout->addWidget(colorMapEditor1DNodeAction->getColorAction().createLabelWidget(this), 0, 0);
    layout->addWidget(colorMapEditor1DNodeAction->getColorAction().createWidget(this), 0, 1);

    auto opacityWidget  = colorMapEditor1DNodeAction->getOpacityAction().createWidget(this);
    auto valueWidget    = colorMapEditor1DNodeAction->getValueAction().createWidget(this);

    dynamic_cast<QDoubleSpinBox*>(opacityWidget->findChild<QDoubleSpinBox*>("SpinBox"))->setMinimumWidth(80);
    dynamic_cast<QDoubleSpinBox*>(valueWidget->findChild<QDoubleSpinBox*>("SpinBox"))->setMinimumWidth(80);

    layout->addWidget(colorMapEditor1DNodeAction->getOpacityAction().createLabelWidget(this), 1, 0);
    layout->addWidget(opacityWidget, 1, 1);

    layout->addWidget(colorMapEditor1DNodeAction->getValueAction().createLabelWidget(this), 2, 0);
    layout->addWidget(valueWidget, 2, 1);

    
    setLayout(layout);
}

}
}
