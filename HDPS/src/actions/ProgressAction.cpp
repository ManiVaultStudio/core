// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProgressAction.h"

#include <QMenu>
#include <QHBoxLayout>

namespace hdps::gui {

ProgressAction::ProgressAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent, title),
    _minimum(0),
    _maximum(100),
    _value(0),
    _textVisible(true),
    _textAlignment(Qt::AlignCenter),
    _textFormat("%p%")
{
    setDefaultWidgetFlags(WidgetFlag::Default);
    setConnectionPermissionsToForceNone(true);
}

ProgressAction::ProgressBarWidget::ProgressBarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags) :
    QProgressBar(parent),
    _progressAction(progressAction)
{
    if (widgetFlags & WidgetFlag::Horizontal)
        setOrientation(Qt::Horizontal);

    if (widgetFlags & WidgetFlag::Vertical)
        setOrientation(Qt::Vertical);

    const auto updateMinimum = [this]() -> void {
        setMinimum(_progressAction->getMinimum());
    };

    updateMinimum();

    connect(_progressAction, &ProgressAction::minimumChanged, this, updateMinimum);

    const auto updateMaximum = [this]() -> void {
        setMaximum(_progressAction->getMaximum());
    };

    updateMaximum();

    connect(_progressAction, &ProgressAction::maximumChanged, this, updateMaximum);

    const auto updateValue = [this]() -> void {
        setValue(_progressAction->getValue());
    };

    updateValue();

    connect(_progressAction, &ProgressAction::valueChanged, this, updateValue);

    const auto updateTextVisible = [this]() -> void {
        setTextVisible(_progressAction->getTextVisible());
    };

    updateTextVisible();

    connect(_progressAction, &ProgressAction::textVisibleChanged, this, updateTextVisible);

    const auto updateTextAlignment = [this]() -> void {
        setAlignment(_progressAction->getTextAlignment());
    };

    updateTextAlignment();

    connect(_progressAction, &ProgressAction::textAlignmentChanged, this, updateTextAlignment);
}

QWidget* ProgressAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new ProgressAction::ProgressBarWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

int ProgressAction::getMinimum() const
{
    return _minimum;
}

void ProgressAction::setMinimum(int minimum)
{
    if (minimum == _minimum)
        return;

    _minimum = std::min(minimum, _maximum);

    emit minimumChanged(_minimum);
}

int ProgressAction::getMaximum() const
{
    return _maximum;
}

void ProgressAction::setMaximum(int maximum)
{
    if (maximum == _maximum)
        return;

    _maximum = std::max(maximum, _minimum);

    emit maximumChanged(_maximum);
}

void ProgressAction::setRange(int minimum, int maximum)
{
    setMinimum(minimum);
    setMaximum(maximum);
}

int ProgressAction::getValue() const
{
    return _value;
}

void ProgressAction::setValue(int value)
{
    if (value == _value)
        return;

    _value = std::clamp(value, _minimum, _maximum);

    emit valueChanged(_value);
}

bool ProgressAction::getTextVisible() const
{
    return _textVisible;
}

void ProgressAction::setTextVisible(bool textVisible)
{
    if (textVisible == _textVisible)
        return;

    _textVisible = textVisible;

    emit textVisibleChanged(_textVisible);
}

Qt::AlignmentFlag ProgressAction::getTextAlignment() const
{
    return _textAlignment;
}

void ProgressAction::setTextAlignment(Qt::AlignmentFlag textAlignment)
{
    if (textAlignment == _textAlignment)
        return;

    _textAlignment = textAlignment;

    emit textAlignmentChanged(_textAlignment);
}

QString ProgressAction::getTextFormat() const
{
    return _textFormat;
}

void ProgressAction::setTextFormat(const QString& textFormat)
{
    if (textFormat == _textFormat)
        return;

    _textFormat = textFormat;

    emit textFormatChanged(_textFormat);
}

}
