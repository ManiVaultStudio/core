// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StringAction.h"

#include "ColorAction.h"

#include <QHBoxLayout>
#include <QCompleter>
#include <QToolButton>
#include <QGraphicsDropShadowEffect>
#include <QEvent>

using namespace mv::util;

namespace mv::gui {

StringAction::StringAction(QObject* parent, const QString& title, const QString& string /*= ""*/) :
    WidgetAction(parent, title),
    _completer(nullptr),
    _searchMode(false),
    _clearable(false),
    _textElideMode(Qt::ElideNone)
{
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setString(string);

    _leadingAction.setVisible(false);
    _trailingAction.setVisible(false);

    _leadingAction.setEnabled(false);

    _leadingAction.setIcon(StyledIcon("magnifying-glass"));
    _trailingAction.setIcon(StyledIcon("xmark"));

    _leadingAction.setToolTip("Search");
    _trailingAction.setToolTip("Clear");
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

QCompleter* StringAction::getCompleter() const
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
        connect(&_trailingAction, &QAction::triggered, this, [this]() -> void {
            setString("");
        });

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

void StringAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicStringAction = dynamic_cast<StringAction*>(publicAction);

    Q_ASSERT(publicStringAction != nullptr);

    if (publicStringAction == nullptr)
        return;

    connect(this, &StringAction::stringChanged, publicStringAction, &StringAction::setString);
    connect(publicStringAction, &StringAction::stringChanged, this, &StringAction::setString);

    setString(publicStringAction->getString());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void StringAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicStringAction = dynamic_cast<StringAction*>(getPublicAction());

    Q_ASSERT(publicStringAction != nullptr);

    if (publicStringAction == nullptr)
        return;

    disconnect(this, &StringAction::stringChanged, publicStringAction, &StringAction::setString);
    disconnect(publicStringAction, &StringAction::stringChanged, this, &StringAction::setString);

    WidgetAction::disconnectFromPublicAction(recursive);
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

QWidget* StringAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::Label)
        layout->addWidget(new StringAction::LabelWidget(parent, this));

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new StringAction::LineEditWidget(parent, this));

    if (widgetFlags & WidgetFlag::TextEdit)
        layout->addWidget(new StringAction::TextEditWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

Qt::TextElideMode StringAction::getTextElideMode() const
{
    return _textElideMode;
}

void StringAction::setTextElideMode(const Qt::TextElideMode& textElideMode)
{
    if (textElideMode == _textElideMode)
        return;

    _textElideMode = textElideMode;

    emit textElideModeChanged(_textElideMode);
}

QRegularExpressionValidator& StringAction::getValidator()
{
    return _validator;
}

QValidator::State StringAction::isValid() const
{
    if (_validator.regularExpression().pattern().isEmpty())
        return QValidator::State::Acceptable;

    int position{};

    auto string = _string;

    return _validator.validate(string, position);
}

StringAction::InlineAction::StyledToolButton::StyledToolButton(InlineAction& inlineAction, QWidget* parent):
	QToolButton(parent),
	_inlineAction(inlineAction)
{
	installEventFilter(this);
    
    inlineAction.installEventFilter(this);

    synchronizeWithInlineAction();
}

bool StringAction::InlineAction::StyledToolButton::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this && _inlineAction.isEnabled()) {
        if (event->type() == QEvent::Enter) {
            addGlowEffect();
        }
        else if (event->type() == QEvent::Leave) {
            removeGlowEffect();
        }
    }

    if (watched == &_inlineAction) {
    	if (event->type() == QEvent::EnabledChange)
            synchronizeWithInlineAction();

    	if (event->type() == QEvent::ToolTipChange)
            synchronizeWithInlineAction();
    }

	return QToolButton::eventFilter(watched, event);
}

void StringAction::InlineAction::StyledToolButton::addGlowEffect()
{
	auto* effect = new QGraphicsDropShadowEffect(this);

    auto color = palette().color(QPalette::ColorRole::Accent);

	effect->setBlurRadius(12);
	effect->setColor(color);
	effect->setOffset(0, 0);

	setGraphicsEffect(effect);
}

void StringAction::InlineAction::StyledToolButton::removeGlowEffect()
{
	setGraphicsEffect(nullptr);
}

void StringAction::InlineAction::StyledToolButton::synchronizeWithInlineAction()
{
    setEnabled(_inlineAction.isEnabled());
    setToolTip(_inlineAction.toolTip());
}

StringAction::InlineAction::InlineAction(QWidget* parent /*= nullptr*/) :
    QWidgetAction(parent)
{
}

QWidget* StringAction::InlineAction::createWidget(QWidget* parent)
{
    auto toolButton = new StyledToolButton(*this, parent);

    toolButton->setIcon(icon());
    toolButton->setIconSize(QSize(12, 12));
    toolButton->setStyleSheet("border: none;");
    toolButton->setAutoRaise(false);
    toolButton->setCursor(Qt::PointingHandCursor);

    connect(toolButton, &QToolButton::clicked, this, [this]() -> void {
        emit triggered();
    });

    return toolButton;
}

void StringAction::InlineAction::setIconByName(const QString& iconName)
{
	setIcon(util::StyledIcon(iconName));
}

StringAction::LabelWidget::LabelWidget(QWidget* parent, StringAction* stringAction) :
    QLabel(parent),
    _stringAction(stringAction)
{
    setObjectName("Label");

    const auto updateToolTip = [this, stringAction]() -> void {
        setToolTip(stringAction->toolTip());
    };

    updateToolTip();

    connect(stringAction, &QAction::changed, this, updateToolTip);

    updateText();

    connect(stringAction, &StringAction::stringChanged, this, &LabelWidget::updateText);
    connect(stringAction, &StringAction::textElideModeChanged, this, &LabelWidget::updateText);

    installEventFilter(this);
}

bool StringAction::LabelWidget::eventFilter(QObject* target, QEvent* event)
{
    if (event->type() == QEvent::Resize)
        updateText();

    return QLabel::eventFilter(target, event);
}

void StringAction::LabelWidget::updateText()
{
    QFontMetrics titleMetrics(font());

    if (_stringAction->getTextElideMode() == Qt::ElideNone)
        setText(_stringAction->getString());
    else
        setText(titleMetrics.elidedText(_stringAction->getString(), _stringAction->getTextElideMode(), width() - 2));
}

StringAction::LineEditWidget::LineEditWidget(QWidget* parent, StringAction* stringAction) :
    QLineEdit(parent),
    _stringAction(stringAction)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);
    setValidator(&_stringAction->getValidator());

    addAction(&_validatorAction, QLineEdit::TrailingPosition);

    const auto updateValidatorAction = [this]() -> void {
        _validatorAction.setVisible(!_stringAction->getString().isEmpty() && !_stringAction->getValidator().regularExpression().pattern().isEmpty());

        if (hasAcceptableInput())
            _validatorAction.setIcon(StyledIcon("check"));
        else
            _validatorAction.setIcon(StyledIcon("exclamation"));
    };

    connect(_stringAction, &StringAction::stringChanged, this, updateValidatorAction);
    connect(&_stringAction->getValidator(), &QRegularExpressionValidator::regularExpressionChanged, this, updateValidatorAction);

    addAction(&_stringAction->getLeadingAction(), QLineEdit::LeadingPosition);
    addAction(&_stringAction->getTrailingAction(), QLineEdit::TrailingPosition);

    const auto updateToolTip = [this, stringAction]() -> void {
        setToolTip(stringAction->getString());
    };

    updateToolTip();

    connect(stringAction, &QAction::changed, this, updateToolTip);

    const auto updatePlaceHolderText = [this]() -> void {
        setPlaceholderText(_stringAction->getPlaceholderString());
    };

    const auto updateText = [this]() -> void {
        const auto cursorPos        = cursorPosition();
        const auto selectionStart   = this->selectionStart();
        const auto selectionLength  = selectedText().length();

        setText(_stringAction->getString());

        if (selectionStart >= 0)
            setSelection(selectionStart, selectionLength);
        else
            setCursorPosition(cursorPos);
    };

    updateText();

    connect(stringAction, &StringAction::stringChanged, this, updateText);
    connect(stringAction, &StringAction::placeholderStringChanged, this, updatePlaceHolderText);

    connect(this, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
        setFocus(Qt::FocusReason::OtherFocusReason);
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

    updatePlaceHolderText();
    updateLeadingAction();
    updateTrailingAction();
    updateCompleter();
    updateValidatorAction();
}

StringAction::TextEditWidget::TextEditWidget(QWidget* parent, StringAction* stringAction) :
    QTextEdit(parent)
{
    setObjectName("TextEdit");
    setAcceptDrops(true);
    
    const auto updateToolTip = [this, stringAction]() -> void {
        setToolTip(stringAction->getString());
    };

    updateToolTip();

    connect(stringAction, &QAction::changed, this, updateToolTip);

    const auto updateTextEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        const auto cacheCursorPosition = textCursor().position();

        setPlainText(stringAction->getString());

        auto updateCursor = textCursor();

        updateCursor.setPosition(cacheCursorPosition);

        setTextCursor(updateCursor);
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
