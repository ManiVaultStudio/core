#include "StringAction.h"
#include "Application.h"

#include <QHBoxLayout>
#include <QCompleter>

using namespace hdps::util;

namespace hdps::gui {

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

QString StringAction::getTypeString() const
{
    return "String";
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
    auto publicStringAction = dynamic_cast<StringAction*>(getPublicAction());

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
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setString(variantMap["Value"].toString());
}

QVariantMap StringAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(getString()) }
    });

    return variantMap;
}

StringAction::LineEditWidget::LineEditWidget(QWidget* parent, StringAction* stringAction) :
    QLineEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);
    setToolTip(stringAction->getString());
    
    const auto updateLineEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        setText(stringAction->getString());
    };

    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        setPlaceholderText(stringAction->getPlaceholderString());
    };

    connect(stringAction, &StringAction::stringChanged, this, updateLineEdit);

    connect(stringAction, &StringAction::placeholderStringChanged, this, updatePlaceHolderText);

    connect(this, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    const auto updateLeadingAction = [this, stringAction]() {
        if (!stringAction->getLeadingAction().isVisible())
            removeAction(&stringAction->getLeadingAction());
        else
            addAction(&stringAction->getLeadingAction(), QLineEdit::LeadingPosition);
    };

    const auto updateTrailingAction = [this, stringAction]() {
        if (!stringAction->getTrailingAction().isVisible())
            removeAction(&stringAction->getTrailingAction());
        else
            addAction(&stringAction->getTrailingAction(), QLineEdit::TrailingPosition);
    };

    connect(&stringAction->getLeadingAction(), &QAction::changed, this, updateLeadingAction);
    connect(&stringAction->getTrailingAction(), &QAction::changed, this, updateTrailingAction);

    const auto updateCompleter = [this, stringAction]() -> void {
        setCompleter(stringAction->getCompleter());
    };

    connect(stringAction, &StringAction::completerChanged, this, updateCompleter);

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

    if (widgetFlags & WidgetFlag::TextEdit)
        layout->addWidget(new StringAction::TextEditWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

StringAction::TextEditWidget::TextEditWidget(QWidget* parent, StringAction* stringAction) :
    QTextEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);

    const auto updateTextEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        setText(stringAction->getString());

        auto cursor = textCursor();

        cursor.movePosition(QTextCursor::End);

        setTextCursor(cursor);
    };

    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        setPlaceholderText(stringAction->getPlaceholderString());
    };

    connect(stringAction, &StringAction::stringChanged, this, updateTextEdit);

    connect(stringAction, &StringAction::placeholderStringChanged, this, updatePlaceHolderText);

    connect(this, &QTextEdit::textChanged, this, [this, stringAction]() {
        stringAction->setString(toPlainText());
    });

    updateTextEdit();
    updatePlaceHolderText();
}

}
