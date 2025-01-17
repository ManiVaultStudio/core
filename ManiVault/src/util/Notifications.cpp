// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notifications.h"

namespace mv::util
{

Notifications::Notifications(QWidget* parent, Position position) :
	QObject(parent),
	_parentWidget(parent),
	_position(position)
{
}

void Notifications::showMessage(const QString& message)
{
    Q_ASSERT(_parentWidget);

    if (!_parentWidget)
        return;

	auto notification = new Notification(message, _parentWidget);

    positionNotification(notification);

	notification->show();

	// Keep track of active toasters
	_notifications.append(notification);

	// Remove toaster from active list when finished
	connect(notification, &Notification::finished, this, [this, notification]() {
		_notifications.removeOne(notification);
		adjustNotificationPositions();
	});
}

void Notifications::setParentWidget(QWidget* parentWidget)
{
    _parentWidget = parentWidget;
}

std::int32_t Notifications::getNumberOfActiveNotifications() const
{
    return static_cast<std::int32_t>(std::count_if(_notifications.begin(), _notifications.end(), [](auto notification) -> bool {
        return !notification->isClosing();
	}));
}

void Notifications::positionNotification(Notification* notification) const
{
    Q_ASSERT(_parentWidget);

    if (!_parentWidget || notification->isClosing())
        return;

	constexpr int margin = 10; // Space between toasters

    const QPoint position(0, _parentWidget->height() - (static_cast<int>(getNumberOfActiveNotifications()) + 1) * (notification->height() + margin) - 50);

    notification->move(_parentWidget->mapToGlobal(position));
}

void Notifications::adjustNotificationPositions()
{
    Q_ASSERT(_parentWidget);

    if (!_parentWidget)
        return;

    for (auto notification : _notifications)
        positionNotification(notification);
}

}