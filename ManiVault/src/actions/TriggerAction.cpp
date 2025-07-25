// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TriggerAction.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QResizeEvent>
#include <QPainter>

namespace mv::gui {

TriggerAction::TriggerAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title)
{
    setDefaultWidgetFlags(WidgetFlag::Text);
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
}

void TriggerAction::selfTriggered()
{
    auto publicTriggerAction = dynamic_cast<TriggerAction*>(getPublicAction());

    if (publicTriggerAction == nullptr)
        return;

    disconnect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);
    {
        publicTriggerAction->trigger();
    }
    connect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);
}

TriggerAction::PushButtonWidget::PushButtonWidget(QWidget* parent, TriggerAction* triggerAction, const std::int32_t& widgetFlags) :
    QPushButton(parent),
    _triggerAction(triggerAction),
    _widgetFlags(widgetFlags)
{
    setIconSize(QSize(12, 12));

    connect(this, &QPushButton::clicked, this, [this, triggerAction]() {
        triggerAction->trigger();
	});

    const auto update = [this, triggerAction, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(triggerAction->isEnabled());

        if (widgetFlags & WidgetFlag::Text) {
            setText(triggerAction->text());
            
            if (triggerAction->text().isEmpty())
                setText(" ");
        }
        
        if (widgetFlags & WidgetFlag::Icon) {
            if ((widgetFlags & WidgetFlag::Text) == 0)
                setText(" ");
        }

        setToolTip(triggerAction->toolTip());
        setVisible(triggerAction->isVisible());
	};

    connect(triggerAction, &QAction::changed, this, [this, update]() {
        update();
	});

    update();
}

void TriggerAction::PushButtonWidget::resizeEvent(QResizeEvent* event)
{
    //QPushButton::resizeEvent(event);
    //return;
    if (_widgetFlags & WidgetFlag::Icon && (_widgetFlags & WidgetFlag::Text) == 0)
		setFixedSize(sizeHint().height(), sizeHint().height());
    else
		QPushButton::resizeEvent(event);
}

void TriggerAction::PushButtonWidget::paintEvent(QPaintEvent* event) {
    QPushButton::paintEvent(event);
    
    if (_widgetFlags & WidgetFlag::Icon && (_widgetFlags & WidgetFlag::Text) == 0) {
        QPainter painter(this);
        
        auto rect = QRect({}, iconSize());

        rect.moveCenter(geometry().center());

        painter.drawPixmap(rect, _triggerAction->icon().pixmap(iconSize(), isEnabled() ? QIcon::Mode::Normal : QIcon::Mode::Disabled));
    }
}

QWidget* TriggerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(new TriggerAction::PushButtonWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

void TriggerAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    auto publicTriggerAction = dynamic_cast<TriggerAction*>(publicAction);

    Q_ASSERT(publicTriggerAction != nullptr);

    if (publicTriggerAction == nullptr)
        return;

    connect(this, &TriggerAction::triggered, this, &TriggerAction::selfTriggered);
    connect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);

    WidgetAction::connectToPublicAction(publicAction, recursive);
}

void TriggerAction::disconnectFromPublicAction(bool recursive)
{
    if (!isConnected())
        return;

    auto publicTriggerAction = dynamic_cast<TriggerAction*>(getPublicAction());

    Q_ASSERT(publicTriggerAction != nullptr);

    if (publicTriggerAction == nullptr)
        return;

    disconnect(this, &TriggerAction::triggered, this, &TriggerAction::selfTriggered);
    disconnect(publicTriggerAction, &TriggerAction::triggered, this, &TriggerAction::trigger);

    WidgetAction::disconnectFromPublicAction(recursive);
}

void TriggerAction::setIconByName(const QString& iconName)
{
	WidgetAction::setIconByName(iconName);

    setDefaultWidgetFlag(WidgetFlag::Icon);
}

void TriggerAction::setIconByName(const QString& iconName, const QString& iconFontName, const util::Version& iconFontVersion)
{
	WidgetAction::setIconByName(iconName, iconFontName, iconFontVersion);

    setDefaultWidgetFlag(WidgetFlag::Icon);
}
}
