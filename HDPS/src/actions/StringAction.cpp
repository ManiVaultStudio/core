#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QCompleter>

namespace hdps {

namespace gui {

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/, const QString& string /*= ""*/, const QString& defaultString /*= ""*/) :
    WidgetAction(parent),
    _string(),
    _defaultString(),
    _placeholderString(),
    _leadingAction(),
    _trailingAction(),
    _completer(nullptr),
    _searchMode(false),
    _clearable(false)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(string, defaultString);

    _leadingAction.setVisible(false);
    _trailingAction.setVisible(false);
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

    saveToSettings();
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

QAction& StringAction::getLeadingAction()
{
    return _leadingAction;
}

QAction& StringAction::getTrailingAction()
{
    return _trailingAction;
}

QCompleter* StringAction::getCompleter()
{
    return _completer;
}

void StringAction::setCompleter(QCompleter* completer)
{
    _completer = completer;

    emit completerChanged(_completer);
}

bool StringAction::getSearchMode() const
{
    return _searchMode;
}

void StringAction::setSearchMode(bool searchMode)
{
    _searchMode = searchMode;

    _leadingAction.setVisible(_searchMode);
    _leadingAction.setIcon(Application::getIconFont("FontAwesome").getIcon("search"));
    
    setClearable(searchMode);
}

bool StringAction::isClearable() const
{
    return _clearable;
}

void StringAction::setClearable(bool clearable)
{
    if (clearable == _clearable)
        return;

    _clearable = clearable;

    _trailingAction.setVisible(false);

    if (_clearable) {
        _trailingAction.setIcon(Application::getIconFont("FontAwesome").getIcon("times-circle"));

        connect(&_trailingAction, &QAction::triggered, this, &StringAction::reset);

        const auto updateTrailingActionVisibility = [this]() -> void {
            _trailingAction.setVisible(!_string.isEmpty());
        };

        connect(this, &StringAction::stringChanged, this, updateTrailingActionVisibility);

        updateTrailingActionVisibility();
    }
    else {
        disconnect(&_trailingAction, &QAction::triggered, this, nullptr);
        disconnect(this, &StringAction::stringChanged, this, nullptr);
    }
}

bool StringAction::isResettable()
{
    return _string != _defaultString;
}

void StringAction::reset()
{
    setString(_defaultString);
}

void StringAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicStringAction = dynamic_cast<StringAction*>(publicAction);

    Q_ASSERT(publicStringAction != nullptr);

    connect(this, &StringAction::stringChanged, publicStringAction, &StringAction::setString);
    connect(publicStringAction, &StringAction::stringChanged, this, &StringAction::setString);

    setString(publicStringAction->getString());

    WidgetAction::connectToPublicAction(publicAction);
}

void StringAction::disconnectFromPublicAction()
{
    auto publicStringAction = dynamic_cast<StringAction*>(_publicAction);

    Q_ASSERT(publicStringAction != nullptr);

    disconnect(this, &StringAction::stringChanged, publicStringAction, &StringAction::setString);
    disconnect(publicStringAction, &StringAction::stringChanged, this, &StringAction::setString);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* StringAction::getPublicCopy() const
{
    return new StringAction(parent(), text(), getString(), getDefaultString());
}

void StringAction::fromVariantMap(const QVariantMap& variantMap)
{
    if (!variantMap.contains("value"))
        return;

    setString(variantMap["value"].toString());
}

QVariantMap StringAction::toVariantMap() const
{
    return {
        { "value", QVariant::fromValue(_string) }
    };
}

StringAction::LineEditWidget::LineEditWidget(QWidget* parent, StringAction* stringAction) :
    QLineEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);
    
    // Update the line edit text from the string action
    const auto updateLineEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        setText(stringAction->getString());
    };

    // Update the place holder text in the line edit
    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        setPlaceholderText(stringAction->getPlaceholderString());
    };

    // Update the line edit string when the action string changes
    connect(stringAction, &StringAction::stringChanged, this, updateLineEdit);

    // Update the line edit placeholder string when the action placeholder string changes
    connect(stringAction, &StringAction::placeholderStringChanged, this, updatePlaceHolderText);

    // Update the action string when the line edit text changes
    connect(this, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    // Update the leading action
    const auto updateLeadingAction = [this, stringAction]() {
        if (!stringAction->getLeadingAction().isVisible())
            removeAction(&stringAction->getLeadingAction());
        else
            addAction(&stringAction->getLeadingAction(), QLineEdit::LeadingPosition);
    };

    // Update the trailing action
    const auto updateTrailingAction = [this, stringAction]() {
        if (!stringAction->getTrailingAction().isVisible())
            removeAction(&stringAction->getTrailingAction());
        else
            addAction(&stringAction->getTrailingAction(), QLineEdit::TrailingPosition);
    };

    // Update leading/trailing action when they change
    connect(&stringAction->getLeadingAction(), &QAction::changed, this, updateLeadingAction);
    connect(&stringAction->getTrailingAction(), &QAction::changed, this, updateTrailingAction);

    // Update the completer
    const auto updateCompleter = [this, stringAction]() -> void {
        setCompleter(stringAction->getCompleter());
    };

    // Update completer when action completer changes
    connect(stringAction, &StringAction::completerChanged, this, updateCompleter);

    // Perform initial updates
    updateLineEdit();
    updatePlaceHolderText();
    updateLeadingAction();
    updateTrailingAction();
    updateCompleter();
}

QWidget* StringAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new StringAction::LineEditWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

}
}
