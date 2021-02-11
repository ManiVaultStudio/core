#include "BooleanAction.h"

namespace hdps {

namespace gui {

BooleanAction::BooleanAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QWidget* BooleanAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
}

BooleanAction::Widget::Widget(QWidget* widget, BooleanAction* optionAction) :
    WidgetAction::Widget(widget, optionAction),
    _layout(),
    _checkbox()
{
    _layout.setMargin(0);
    _layout.addWidget(&_checkbox);

    setLayout(&_layout);
}

}
}