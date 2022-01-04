#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

    

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/, const QString& string /*= ""*/, const QString& defaultString /*= ""*/) :
    WidgetAction(parent)
{
    setText(title); 
    setIcon(Application::getIconFont("FontAwesome").getIcon("quote-right"));
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::LineEdit);
    initialize(string, defaultString);
}

void StringAction::initialize(const QString& string /*= ""*/, const QString& defaultString /*= ""*/)
{
    setString(string);
    setDefaultString(defaultString);

    setResettable(isResettable());
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

    setResettable(isResettable());
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

    setResettable(isResettable());
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

StringAction::Widget::Widget(QWidget* parent, StringAction* stringAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, stringAction)
{
    auto layout = new QHBoxLayout();

    if (widgetFlags & StringAction::WidgetFlag::LineEdit)
        layout->addWidget(new StringAction::LineEditWidget(this, stringAction));

    if (widgetFlags & WidgetActionWidget::PopupLayout) {
        setPopupLayout(layout);
    }
    else {
        layout->setMargin(0);
        setLayout(layout);
    }
}

}
}
