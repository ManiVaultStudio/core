#include "StringAction.h"

namespace hdps {

namespace gui {

StringAction::Widget::Widget(QWidget* parent, StringAction* stringAction) :
    WidgetAction::Widget(parent, stringAction),
    _layout(),
    _lineEdit()
{
    _layout.setMargin(0);
    _layout.addWidget(&_lineEdit);

    setLayout(&_layout);

    const auto updateLineEdit = [this, stringAction]() {
        _lineEdit.setText(stringAction->getValue());
    };

    connect(stringAction, &StringAction::valueChanged, this, [this, updateLineEdit](const QString& value) {
        updateLineEdit();
    });

    connect(&_lineEdit, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setValue(text);
    });

    updateLineEdit();
}

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QWidget* StringAction::createWidget(QWidget* parent)
{
    auto widget = new Widget(parent, this);

    widget->show();

    return widget;
}

QString StringAction::getValue() const
{
    return _value;
}

void StringAction::setValue(const QString& value)
{
    if (value == _value)
        return;

    _value = value;

    emit valueChanged(_value);
}

}
}