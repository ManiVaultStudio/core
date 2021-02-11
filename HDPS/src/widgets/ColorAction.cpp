#include "ColorAction.h"

#include <QDebug>

namespace hdps {

namespace gui {

ColorAction::ColorAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _color(Qt::black)
{
    setText(title);
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
}

}
}