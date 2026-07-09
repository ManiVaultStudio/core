// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notifications.h"

#include "Application.h"

#include <QMainWindow>
#include <QEvent>
#include <QTimer>

namespace mv::util
{

Notifications::Notifications(QWidget* parent) :
    QObject(parent)
{
}

void Notifications::showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs)
{
    if (Application::getMainWindow()) {
	    const auto createNotification = [this, title, description, icon, durationType]() -> Notification* {
	    	return new Notification(title, description, icon, _notifications.isEmpty() ? nullptr : _notifications.last(), durationType, Application::getMainWindow());
	    };

    	if (delayMs > 0) {
    		QTimer::singleShot(delayMs, this, [this, createNotification]() {
				addNotification(createNotification());
				});
    	}
    	else {
    		addNotification(createNotification());
    	}
    }
}

void Notifications::showTask(QPointer<Task> task)
{
    if (auto mainWindow = Application::getMainWindow()) {
		addNotification(new Notification(task, _notifications.isEmpty() ? nullptr : _notifications.last(), mainWindow));
    }
}

void Notifications::setupMainWindowSynchronization()
{
    if (auto mainWindow = Application::getMainWindow()) {
        mainWindow->removeEventFilter(this);
        mainWindow->installEventFilter(this);
    }
}

bool Notifications::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        QTimer::singleShot(10, this, &Notifications::updateAllPositions);
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

        updateAllPositions();
    });

    notification->updatePosition();
    notification->show();

    _notifications.append(notification);
}

void Notifications::updateAllPositions()
{
    for (auto repositionNotification : _notifications)
        repositionNotification->updatePosition();
}

}
