#include "ColorMapEditor1DNodeAction.h"
#include "ColorMapEditor1DNode.h"
#include "ColorMapEditor1DAction.h"
#include "ColorMapAction.h"
#include "ColorMapSettingsAction.h"

#include <QGridLayout>

namespace hdps {

namespace gui {

ColorMapEditor1DNodeAction::ColorMapEditor1DNodeAction(ColorMapEditor1DAction& colorMapEditor1DAction) :
    WidgetAction(&colorMapEditor1DAction),
    _colorMapEditor1DAction(colorMapEditor1DAction),
    _currentNode(nullptr),
    _colorAction(this, "Color", Qt::yellow),
    _opacityAction(this, "Intensity", 0.0f, 100.0f, 0.0f, 0.0f, 0),
    _valueAction(this, "Value", 0.0f, 100.0f, 0.0f, 0.0f, 0)
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

        auto& rangeAction = _colorMapEditor1DAction.getColorMapAction().getSettingsAction().getHorizontalAxisAction().getRangeAction();

        const auto minimum      = rangeAction.getMinimum();
        const auto maximum      = rangeAction.getMaximum();
        const auto length       = maximum - minimum;
        const auto x            = (_valueAction.getValue() - minimum) / length;
        const auto y            = 0.01f * _opacityAction.getValue();

        _currentNode->setNormalizedCoordinate(QPointF(x, y));
    };

    connect(&_valueAction, &DecimalAction::valueChanged, this, updateNodeNormalizedCoordinate);
    connect(&_opacityAction, &DecimalAction::valueChanged, this, updateNodeNormalizedCoordinate);
}

void ColorMapEditor1DNodeAction::connectToNode(ColorMapEditor1DNode* node)
{
    Q_ASSERT(node != nullptr);

    if (_currentNode)
        disconnectFromNode(_currentNode);

    _currentNode = node;

    const auto enable = _currentNode != nullptr;

    _colorAction.setEnabled(enable);
    _opacityAction.setEnabled(enable);
    _valueAction.setEnabled(enable);

    connect(_currentNode, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, &ColorMapEditor1DNodeAction::nodeChanged);

    nodeChanged();
}

void ColorMapEditor1DNodeAction::disconnectFromNode(ColorMapEditor1DNode* node)
{
    Q_ASSERT(node != nullptr);

    disconnect(node, &ColorMapEditor1DNode::normalizedCoordinateChanged, this, nullptr);

    _currentNode = nullptr;
}

void ColorMapEditor1DNodeAction::changeNodePosition(float xPosition, float yPosition)
{
    _valueAction.setValue(xPosition);
    _opacityAction.setValue(yPosition);
}

void ColorMapEditor1DNodeAction::changeNodeColor(const QColor& color)
{
    _colorAction.setColor(color);
}

void ColorMapEditor1DNodeAction::nodeChanged()
{
    auto& rangeAction = _colorMapEditor1DAction.getColorMapAction().getSettingsAction().getHorizontalAxisAction().getRangeAction();

    const auto minimum      = rangeAction.getMinimum();
    const auto maximum      = rangeAction.getMaximum();
    const auto length       = maximum - minimum;
    const auto coordinate   = _currentNode->getNormalizedCoordinate();
    const auto x            = minimum + (coordinate.x() * length);
    const auto y            = coordinate.y();

    _valueAction.initialize(minimum, maximum, x, x, 1);
    _opacityAction.setValue(y * 100.0f);
}

ColorMapEditor1DNodeAction::Widget::Widget(QWidget* parent, ColorMapEditor1DNodeAction* colorMapEditor1DNodeAction) :
    WidgetActionWidget(parent, colorMapEditor1DNodeAction)
{
    auto layout = new QGridLayout();

    layout->setMargin(0);
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 3);

    layout->addWidget(colorMapEditor1DNodeAction->getColorAction().createLabelWidget(this), 0, 0);
    layout->addWidget(colorMapEditor1DNodeAction->getColorAction().createWidget(this), 0, 1);

    layout->addWidget(colorMapEditor1DNodeAction->getOpacityAction().createLabelWidget(this), 1, 0);
    layout->addWidget(colorMapEditor1DNodeAction->getOpacityAction().createWidget(this), 1, 1);

    layout->addWidget(colorMapEditor1DNodeAction->getValueAction().createLabelWidget(this), 2, 0);
    layout->addWidget(colorMapEditor1DNodeAction->getValueAction().createWidget(this), 2, 1);

    setLayout(layout);
}

}
}
