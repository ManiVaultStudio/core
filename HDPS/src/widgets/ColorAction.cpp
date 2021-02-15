#include "ColorAction.h"

#include <QDebug>

namespace hdps {

namespace gui {

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/, const QColor& color /*= Qt::gray*/) :
    WidgetAction(parent),
    _color()
{
    setText(title);
    setColor(color);
}

QWidget* ColorAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
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

ColorAction::Widget::Widget(QWidget* parent, ColorAction* colorAction) :
    WidgetAction::Widget(parent, colorAction),
    _layout(),
    _colorPickerPushButton(this)
{
    _layout.setMargin(0);
    _layout.addWidget(&_colorPickerPushButton);

    setLayout(&_layout);

    connect(&_colorPickerPushButton, &ColorPickerPushButton::colorChanged, this, [this, colorAction](const QColor& color) {
        colorAction->setColor(color);
    });

    const auto updateColorPickerPushButton = [this, colorAction]() -> void {
        _colorPickerPushButton.setColor(colorAction->getColor());
    };

    connect(colorAction, &ColorAction::colorChanged, this, [this, updateColorPickerPushButton](const QColor& color) {
        updateColorPickerPushButton();
    });

    updateColorPickerPushButton();
}

}
}