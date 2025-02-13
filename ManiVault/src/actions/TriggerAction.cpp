// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TriggerAction.h"

#include <QHBoxLayout>
#include <QMenu>
#include <QResizeEvent>

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
#ifdef __APPLE__
    setIconSize(QSize(12, 12));
#endif
    
    connect(this, &QPushButton::clicked, this, [this, triggerAction]() {
        triggerAction->trigger();
	});

    const auto update = [this, triggerAction, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(triggerAction->isEnabled());

        if (widgetFlags & WidgetFlag::Text) {
            setText(triggerAction->text());
            
#ifdef __APPLE__
            
            // This is a work-around to prevent sizing issues with pushbuttons that have only an icon
            if (triggerAction->text().isEmpty())
                setText(" ");
#endif
        }
        
        if (widgetFlags & WidgetFlag::Icon) {
#ifdef __APPLE__
            
            // This is a work-around to prevent sizing issues with pushbuttons that have only an icon
            if ((widgetFlags & WidgetFlag::Text) == 0)
                setText(" ");
#else
            setIcon(triggerAction->icon());
#endif
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
    if (_widgetFlags & WidgetFlag::Icon && (_widgetFlags & WidgetFlag::Text) == 0)
		setFixedSize(event->size().height(), event->size().height());
    else
		QPushButton::resizeEvent(event);
}

void TriggerAction::PushButtonWidget::paintEvent(QPaintEvent* event) {
    QPushButton::paintEvent(event);
    
    if (_widgetFlags & WidgetFlag::Icon && (_widgetFlags & WidgetFlag::Text) == 0) {
        QPainter painter(this);
        
        const auto iconSize = this->iconSize();
        const auto rect     = this->rect();
        const auto center   = rect.center() - QPoint(iconSize.width() / 2, iconSize.height() / 2) + QPoint(1, 1);
        
        _triggerAction->icon().paint(&painter, QRect(center, iconSize));
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

}
