// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notifications.h"

#include <QMainWindow>

namespace mv::util
{

Notifications::Notifications(QWidget* parent) :
	QObject(parent),
	_parentWidget(parent)
{
}

void Notifications::showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs)
{
    //Q_ASSERT(_parentWidget);

    if (!_parentWidget)
        return;

    return;

    const auto addNotification = [this, title, description, icon, durationType]() -> void {
        auto notification = new Notification(title, description, icon, _notifications.isEmpty() ? nullptr : _notifications.last(), durationType, _parentWidget);

        notification->show();

        _notifications.append(notification);

        connect(notification, &Notification::finished, this, [this, notification]() {
            _notifications.removeOne(notification);
            notification->deleteLater();
		});
    };

    if (delayMs > 0)
        QTimer::singleShot(delayMs, addNotification);
    else
        addNotification();
}

void Notifications::setParentWidget(QWidget* parentWidget)
{
    _parentWidget = parentWidget;

    Application::getMainWindow()->installEventFilter(this);
}

bool Notifications::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        for (auto notification : _notifications)
            QTimer::singleShot(10, notification, &Notification::updatePosition);
    }

	return QObject::eventFilter(watched, event);
}

}
