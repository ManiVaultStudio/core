#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QLineEdit>

namespace hdps {

namespace gui {

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/, const QString& string /*= ""*/, const QString& defaultString /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
    initialize(string, defaultString);
}

void StringAction::initialize(const QString& string /*= ""*/, const QString& defaultString /*= ""*/)
{
    setString(string);
    setDefaultString(defaultString);
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

QString StringAction::getPlaceholderString() const
{
    return _placeholderString;
}

void StringAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == _placeholderString)
        return;

    _placeholderString = placeholderString;

    emit placeholderStringChanged(_placeholderString);
}

StringAction::LineEditWidget::LineEditWidget(QWidget* parent, StringAction* stringAction) :
    WidgetActionWidget(parent, stringAction, WidgetActionWidget::State::Standard),
    _lineEdit(new QLineEdit())
{
    setAcceptDrops(true);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->addWidget(_lineEdit);

    setLayout(layout);

    const auto updateLineEdit = [this, stringAction]() {
        QSignalBlocker blocker(_lineEdit);

        _lineEdit->setText(stringAction->getString());
    };

    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        _lineEdit->setPlaceholderText(stringAction->getPlaceholderString());
    };

    connect(stringAction, &StringAction::stringChanged, this, [this, updateLineEdit](const QString& value) {
        updateLineEdit();
    });

    connect(stringAction, &StringAction::placeholderStringChanged, this, [this, updatePlaceHolderText](const QString& value) {
        updatePlaceHolderText();
    });

    connect(_lineEdit, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    updateLineEdit();
    updatePlaceHolderText();
}

}
}
