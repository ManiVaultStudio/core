#include "StringAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

    

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/, const QString& string /*= ""*/, const QString& defaultString /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
    setWidgetFlags(WidgetFlag::Basic);
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

bool StringAction::isResettable() const
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
    QLineEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);
    
    const auto update = [this, stringAction]() -> void {
        setEnabled(stringAction->isEnabled());
        setToolTip(stringAction->text());
    };

    connect(stringAction, &StringAction::changed, this, [update]() {
        update();
    });

    update();

    const auto updateLineEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        setText(stringAction->getString());
    };

    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        setPlaceholderText(stringAction->getPlaceholderString());
    };

    connect(stringAction, &StringAction::stringChanged, this, [this, updateLineEdit](const QString& value) {
        updateLineEdit();
    });

    connect(stringAction, &StringAction::placeholderStringChanged, this, [this, updatePlaceHolderText](const QString& value) {
        updatePlaceHolderText();
    });

    connect(this, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    updateLineEdit();
    updatePlaceHolderText();
}

QWidget* StringAction::getWidget(QWidget* parent, const WidgetActionWidget::State& state /*= WidgetActionWidget::State::Standard*/)
{
    auto widget = new QWidget(parent);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (hasWidgetFlag(WidgetFlag::LineEdit))
        layout->addWidget(new StringAction::LineEditWidget(parent, this));

    if (hasWidgetFlag(WidgetFlag::ResetPushButton))
        layout->addWidget(createResetButton(parent));

    widget->setLayout(layout);

    const auto update = [this, widget]() -> void {
        widget->setEnabled(isEnabled());
        widget->setToolTip(text());
    };

    connect(this, &StringAction::changed, this, [update]() {
        update();
    });

    update();

    return widget;
}

}
}
