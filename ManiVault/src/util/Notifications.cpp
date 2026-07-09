// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notifications.h"

#include "Application.h"

#include <QMainWindow>
#include <QEvent>

namespace mv::util
{

Notifications::Notifications(QWidget* parent) :
    QObject(parent)
{
    Q_ASSERT(parent);
}

void Notifications::showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs)
{
    const auto createNotification = [&]() -> Notification* {
	    return new Notification(title, description, icon, _notifications.isEmpty() ? nullptr : _notifications.last(), durationType, Application::getMainWindow());
    };

    if (delayMs > 0) {
	    QTimer::singleShot(delayMs, [this, createNotification]() {
			addNotification(createNotification());
		});
    }
    else {
	    addNotification(createNotification());
    }
}

void Notifications::showTask(QPointer<Task> task)
{
    addNotification(new Notification(task, _notifications.isEmpty() ? nullptr : _notifications.last(), Application::getMainWindow()));
}

bool Notifications::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        for (auto notification : _notifications)
            QTimer::singleShot(10, notification, &Notification::updatePosition);
    }

    return QObject::eventFilter(watched, event);
}

void Notifications::addNotification(Notification* notification)
{
    Q_ASSERT(notification);

    if (!notification)
        return;

    connect(notification, &Notification::linkActivated, this, &Notifications::notificationLinkActivated);
    connect(notification, &Notification::finished, this, [this, notification]() {
        _notifications.removeOne(notification);
        notification->deleteLater();

        for (auto repositionNotification : _notifications)
            repositionNotification->updatePosition();
    });

    notification->updatePosition();
    notification->show();

    _notifications.append(notification);
}

}
