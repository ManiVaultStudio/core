// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ToggleAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QEvent>
#include <QMouseEvent>
#include <QLabel>

using namespace mv::util;

namespace mv::gui {

ToggleAction::ToggleAction(QObject* parent, const QString& title, bool toggled /*= false*/) :
    WidgetAction(parent, title),
    _indeterminate(false)
{
    setCheckable(true);
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    setChecked(toggled);
}

void ToggleAction::setChecked(bool checked)
{
    QAction::setChecked(checked);

    _indeterminate = false;

    saveToSettings();
}

bool ToggleAction::getIndeterminate() const
{
    return _indeterminate;
}

void ToggleAction::setIndeterminate(bool indeterminate)
{
    if (indeterminate == _indeterminate)
        return;

    _indeterminate = indeterminate;

    emit indeterminateChanged(_indeterminate);
}

void ToggleAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "Value");

    setChecked(variantMap["Value"].toBool());
}

QVariantMap ToggleAction::toVariantMap() const
{
    auto variantMap = WidgetAction::toVariantMap();

    variantMap.insert({
        { "Value", QVariant::fromValue(isChecked()) }
    });

    return variantMap;
}

void ToggleAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicToggleAction = dynamic_cast<ToggleAction*>(publicAction);

    Q_ASSERT(publicToggleAction != nullptr);

    if (publicToggleAction == nullptr)
        return;

    connect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    connect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    setChecked(publicToggleAction->isChecked());

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void ToggleAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicToggleAction = dynamic_cast<ToggleAction*>(getPublicAction());

    Q_ASSERT(publicToggleAction != nullptr);

    if (publicToggleAction == nullptr)
        return;

    disconnect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    disconnect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    WidgetAction::disconnectFromPublicAction(recursive);
}

ToggleAction::CheckBoxWidget::CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction) :
    QCheckBox(parent),
    _toggleAction(toggleAction)
{
    setAcceptDrops(true);
    setText("");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(18, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    auto labelWidget = toggleAction->createLabelWidget(this, 0);
    
    labelWidget->installEventFilter(this);

    layout->addWidget(labelWidget);
    layout->addStretch(1);

    setLayout(layout);

    connect(this, &QCheckBox::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(_toggleAction->isEnabled());
        setToolTip(_toggleAction->toolTip());
        setVisible(_toggleAction->isVisible());
    };

    connect(toggleAction, &ToggleAction::changed, this, [this, update]() {
        update();
    });

    const auto updateToggle = [this]() -> void {
        QSignalBlocker blocker(this);

        if (_toggleAction->getIndeterminate())
            setCheckState(Qt::CheckState::PartiallyChecked);
        else
            setChecked(_toggleAction->isChecked());
    };

    connect(_toggleAction, &ToggleAction::toggled, this, [this, updateToggle]() {
        updateToggle();
    });

    connect(_toggleAction, &ToggleAction::indeterminateChanged, this, [this, updateToggle]() {
        updateToggle();
    });

    update();
    updateToggle();
}

bool ToggleAction::CheckBoxWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = dynamic_cast<QMouseEvent*>(event);

            if (_toggleAction->isEnabled() && mouseEvent->button() == Qt::LeftButton && !_toggleAction->getDrag().isDragging())
            {
                _toggleAction->setChecked(!_toggleAction->isChecked());
            }
            
            return true;
        }

        default:
            break;
    }

    return QCheckBox::eventFilter(target, event);
}

ToggleAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags) :
    QPushButton(parent),
    _toggleAction(toggleAction),
    _widgetFlags(widgetFlags)
{
    setCheckable(true);

    connect(this, &QPushButton::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        if (isEnabled() != _toggleAction->isEnabled())
            setEnabled(_toggleAction->isEnabled());

        if (isChecked() != _toggleAction->isChecked())
            setChecked(_toggleAction->isChecked());

        if (toolTip() != _toggleAction->toolTip())
            setToolTip(_toggleAction->toolTip());

        if (isVisible() != _toggleAction->isVisible())
            setVisible(_toggleAction->isVisible());

        if (widgetFlags & WidgetFlag::Text && text() != _toggleAction->text())
            setText(_toggleAction->text());

        if (widgetFlags & WidgetFlag::Icon)
            setIcon(_toggleAction->icon());
    };

    connect(_toggleAction, &QAction::changed, this, update);

    update();
}

void ToggleAction::PushButtonWidget::resizeEvent(QResizeEvent* event)
{
    if (_widgetFlags & WidgetFlag::Icon && (_widgetFlags & WidgetFlag::Text) == 0)
        setFixedSize(event->size().height(), event->size().height());
    else
        QPushButton::resizeEvent(event);
}

ToggleAction::ToggleImageLabelWidget::ToggleImageLabelWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags) :
    QWidget(parent),
    _toggleAction(toggleAction)
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setAlignment(Qt::AlignVCenter);

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    auto imageLabel = new QLabel(this);

    QLabel* titleLabel = nullptr;

    layout->addWidget(imageLabel);
    
    if (widgetFlags & WidgetFlag::Text)
    {
        titleLabel = new QLabel(_toggleAction->text(), this);

        layout->addWidget(titleLabel);

        const auto updateTitle = [this, titleLabel]() -> void {
            titleLabel->setText(_toggleAction->text());
		};

        updateTitle();

        connect(_toggleAction, &ToggleAction::textChanged, this, updateTitle);
    }

    const auto updatePixmap = [this, imageLabel, titleLabel]() -> void {
        imageLabel->setPixmap(QIcon(StyledIcon(_toggleAction->isChecked() ? "toggle-on" : "toggle-off")).pixmap(QSize(20, 20)));

    	imageLabel->setToolTip(_toggleAction->toolTip());

        if (titleLabel)
			titleLabel->setToolTip(_toggleAction->toolTip());
	};

    updatePixmap();

    connect(_toggleAction, &ToggleAction::changed, this, updatePixmap);
}

void ToggleAction::ToggleImageLabelWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    if (_toggleAction->isEnabled() && event->button() == Qt::LeftButton && !_toggleAction->getDrag().isDragging())
        _toggleAction->setChecked(!_toggleAction->isChecked());
}

QWidget* ToggleAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::CheckBox)
        layout->addWidget(new ToggleAction::CheckBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(new ToggleAction::PushButtonWidget(parent, this, widgetFlags));

    if (widgetFlags & WidgetFlag::ToggleImage)
        layout->addWidget(new ToggleAction::ToggleImageLabelWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
