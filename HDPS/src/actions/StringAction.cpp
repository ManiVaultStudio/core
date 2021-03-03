#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QLineEdit>

namespace hdps {

namespace gui {

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
}

QWidget* StringAction::createWidget(QWidget* parent)
{
    return new Widget(parent, this);
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

StringAction::Widget::Widget(QWidget* parent, StringAction* stringAction, const bool& resettable /*= true*/) :
    WidgetAction::Widget(parent, stringAction)
{
    auto layout = new QHBoxLayout();

    auto lineEdit = new QLineEdit();

    layout->setMargin(0);
    layout->addWidget(lineEdit);

    setLayout(layout);

    const auto updateLineEdit = [this, stringAction, lineEdit]() {
        lineEdit->setText(stringAction->getString());
    };

    connect(stringAction, &StringAction::stringChanged, this, [this, updateLineEdit](const QString& value) {
        updateLineEdit();
    });

    connect(lineEdit, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    if (resettable) {
        auto resetPushButton = new QPushButton();

        resetPushButton->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("undo"));
        resetPushButton->setToolTip(QString("Reset %1").arg(stringAction->text()));

        layout->addWidget(resetPushButton);

        connect(resetPushButton, &QPushButton::clicked, this, [this, stringAction]() {
            stringAction->reset();
        });

        const auto onUpdateString = [this, stringAction, resetPushButton]() -> void {
            resetPushButton->setEnabled(stringAction->canReset());
        };

        connect(stringAction, &StringAction::stringChanged, this, [this, onUpdateString](const QColor& color) {
            onUpdateString();
        });

        onUpdateString();
    }

    updateLineEdit();
}

}
}