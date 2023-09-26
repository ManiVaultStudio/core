// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProgressAction.h"

#include <QMenu>
#include <QHBoxLayout>
#include <QStyleOptionProgressBar>
#include <QPainter>

namespace hdps::gui {

ProgressAction::ProgressAction(QObject* parent, const QString& title) :
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

int ProgressAction::getProgress() const
{
    return _value;
}

void ProgressAction::setProgress(int value)
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
    if (!_overrideTextFormat.isEmpty())
        return _overrideTextFormat;

    return _textFormat;
}

void ProgressAction::setTextFormat(const QString& textFormat)
{
    if (textFormat == _textFormat)
        return;

    const auto previousTextFormat = _textFormat;

    _textFormat = textFormat;

    emit textFormatChanged(previousTextFormat, _textFormat);
}

QString ProgressAction::getOverrideTextFormat() const
{
    return _overrideTextFormat;
}

void ProgressAction::setOverrideTextFormat(const QString& overrideTextFormat)
{
    if (overrideTextFormat == _overrideTextFormat)
        return;

    const auto previousOverrideTextFormat = _overrideTextFormat;

    _overrideTextFormat = overrideTextFormat;

    emit overrideTextFormatChanged(previousOverrideTextFormat, _overrideTextFormat);
}

ProgressAction::BarWidget::BarWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags) :
    QProgressBar(parent),
    _progressAction(progressAction)
{
    setStyleSheet("QProgressBar { font-size: 10px; }");

    if (widgetFlags & WidgetFlag::HorizontalBar)
        setOrientation(Qt::Horizontal);

    if (widgetFlags & WidgetFlag::VerticalBar)
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
        setValue(_progressAction->getProgress());
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

    const auto updateTextFormat = [this]() -> void {
        setFormat(_progressAction->getTextFormat());
    };

    updateTextFormat();

    connect(_progressAction, &ProgressAction::textFormatChanged, this, updateTextFormat);
    connect(_progressAction, &ProgressAction::overrideTextFormatChanged, this, updateTextFormat);
}

void ProgressAction::BarWidget::paintEvent(QPaintEvent* paintEvent)
{
    QStyleOptionProgressBar progressBarStyleOption;
    
    initStyleOption(&progressBarStyleOption);

    progressBarStyleOption.text = progressBarStyleOption.fontMetrics.elidedText(QString("  %1  ").arg(_progressAction->getText()), Qt::ElideMiddle, progressBarStyleOption.rect.width());

    QPainter painter(this);
    
    style()->drawControl(QStyle::CE_ProgressBar, &progressBarStyleOption, &painter, this);
}

ProgressAction::LabelWidget::LabelWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags) :
    QLabel(parent),
    _progressAction(progressAction)
{
    const auto updateText = [this]() -> void {
        setText(_progressAction->getText());
    };

    updateText();

    connect(_progressAction, &ProgressAction::minimumChanged, this, updateText);
    connect(_progressAction, &ProgressAction::maximumChanged, this, updateText);
    connect(_progressAction, &ProgressAction::numberOfStepsChanged, this, updateText);
    connect(_progressAction, &ProgressAction::valueChanged, this, updateText);
    connect(_progressAction, &ProgressAction::textFormatChanged, this, updateText);
}

ProgressAction::LineEditWidget::LineEditWidget(QWidget* parent, ProgressAction* progressAction, const std::int32_t& widgetFlags) :
    QLineEdit(parent),
    _progressAction(progressAction)
{
    setEnabled(false);

    const auto updateText = [this]() -> void {
        setText(_progressAction->getText());
    };

    updateText();

    connect(_progressAction, &ProgressAction::minimumChanged, this, updateText);
    connect(_progressAction, &ProgressAction::maximumChanged, this, updateText);
    connect(_progressAction, &ProgressAction::numberOfStepsChanged, this, updateText);
    connect(_progressAction, &ProgressAction::valueChanged, this, updateText);
    connect(_progressAction, &ProgressAction::textFormatChanged, this, updateText);
}

QWidget* ProgressAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if ((widgetFlags & ProgressAction::WidgetFlag::HorizontalBar) || (widgetFlags & ProgressAction::WidgetFlag::VerticalBar))
        layout->addWidget(new ProgressAction::BarWidget(parent, this, widgetFlags));

    if (widgetFlags & ProgressAction::WidgetFlag::Label)
        layout->addWidget(new ProgressAction::LabelWidget(parent, this, widgetFlags));

    if (widgetFlags & ProgressAction::WidgetFlag::LineEdit)
        layout->addWidget(new ProgressAction::LineEditWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

int ProgressAction::getNumberOfSteps() const
{
    return getMaximum() - getMinimum();
}

float ProgressAction::getPercentage() const
{
    if (getMinimum() == getMaximum())
        return 0.f;

    return 100.f * ((getProgress() - getMinimum()) / static_cast<float>(getNumberOfSteps()));
}

QString ProgressAction::getText() const
{
    auto text = _textFormat;

    text.replace("%p", QString::number(getPercentage(), 'f', 1));
    text.replace("%v", QString::number(getProgress()));
    text.replace("%m", QString::number(getNumberOfSteps()));

    return text;
}

}
