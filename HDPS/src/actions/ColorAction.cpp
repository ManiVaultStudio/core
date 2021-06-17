#include "ColorAction.h"
#include "Application.h"

#include "../widgets/ColorPickerPushButton.h"

#include <QDebug>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

const QColor ColorAction::DEFAULT_COLOR = Qt::gray;

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= DEFAULT_COLOR*/, const QColor& defaultColor /*= DEFAULT_COLOR*/) :
    WidgetAction(parent),
    _color(),
    _defaultColor()
{
    setText(title);
    setColor(color);
    setDefaultColor(defaultColor);
}

QColor ColorAction::getColor() const
{
    return _color;
}

void ColorAction::setColor(const QColor& color)
{
    if (color == _color)
        return;

    _color = color;

    emit colorChanged(_color);
}

QColor ColorAction::getDefaultColor() const
{
    return _defaultColor;
}

void ColorAction::setDefaultColor(const QColor& defaultColor)
{
    if (defaultColor == _defaultColor)
        return;

    _defaultColor = defaultColor;

    emit defaultColorChanged(_defaultColor);
}

bool ColorAction::canReset() const
{
    return _color != _defaultColor;
}

void ColorAction::reset()
{
    setColor(_defaultColor);
}

ColorAction::Widget::Widget(QWidget* parent, ColorAction* colorAction) :
    WidgetAction::Widget(parent, colorAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _colorPickerPushButton(new ColorPickerPushButton()),
    _resetPushButton(new QPushButton())
{
    _layout->setMargin(0);
    _layout->addWidget(_colorPickerPushButton);

    setLayout(_layout);

    connect(_colorPickerPushButton, &ColorPickerPushButton::colorChanged, this, [this, colorAction](const QColor& color) {
        colorAction->setColor(color);
    });

    const auto updateColorPickerPushButton = [this, colorAction]() -> void {
        _colorPickerPushButton->setColor(colorAction->getColor());
    };

    connect(colorAction, &ColorAction::colorChanged, this, [this, updateColorPickerPushButton](const QColor& color) {
        updateColorPickerPushButton();
    });

    updateColorPickerPushButton();

    _resetPushButton->setVisible(false);
    _resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
    _resetPushButton->setToolTip(QString("Reset %1").arg(colorAction->text()));

    _layout->addWidget(_resetPushButton);

    connect(_resetPushButton, &QPushButton::clicked, this, [this, colorAction]() {
        colorAction->reset();
    });

    const auto onUpdateColor = [this, colorAction]() -> void {
        _resetPushButton->setEnabled(colorAction->canReset());
    };

    connect(colorAction, &ColorAction::colorChanged, this, [this, onUpdateColor](const QColor& color) {
        onUpdateColor();
    });

    onUpdateColor();
}

}
}