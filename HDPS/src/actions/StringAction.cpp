#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

namespace hdps {

namespace gui {

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QString StringAction::getString() const
{
    return _string;
}

void StringAction::setString(const QString& string)
{
    if (string == _string)
        return;

    _string = string;

    emit stringChanged(_string);
}

QString StringAction::getDefaultString() const
{
    return _defaultString;
}

void StringAction::setDefaultString(const QString& defaultString)
{
    if (defaultString == _defaultString)
        return;

    _defaultString = defaultString;

    emit defaultStringChanged(_defaultString);
}

bool StringAction::canReset() const
{
    return _string != _defaultString;
}

void StringAction::reset()
{
    setString(_defaultString);
}

StringAction::Widget::Widget(QWidget* parent, StringAction* stringAction) :
    WidgetAction::Widget(parent, stringAction, Widget::State::Standard),
    _layout(new QHBoxLayout()),
    _lineEdit(new QLineEdit()),
    _resetPushButton(new QPushButton())
{
    _layout->setMargin(0);
    _layout->addWidget(_lineEdit);

    setLayout(_layout);

    const auto updateLineEdit = [this, stringAction]() {
        _lineEdit->setText(stringAction->getString());
    };

    connect(stringAction, &StringAction::stringChanged, this, [this, updateLineEdit](const QString& value) {
        updateLineEdit();
    });

    connect(_lineEdit, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    _resetPushButton->setVisible(false);
    _resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
    _resetPushButton->setToolTip(QString("Reset %1").arg(stringAction->text()));

    _layout->addWidget(_resetPushButton);

    connect(_resetPushButton, &QPushButton::clicked, this, [this, stringAction]() {
        stringAction->reset();
    });

    const auto onUpdateString = [this, stringAction]() -> void {
        _resetPushButton->setEnabled(stringAction->canReset());
    };

    connect(stringAction, &StringAction::stringChanged, this, [this, onUpdateString](const QColor& color) {
        onUpdateString();
    });

    onUpdateString();
    updateLineEdit();
}

}
}