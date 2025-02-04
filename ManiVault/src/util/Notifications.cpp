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

	auto notification = new Notification(message + QString::number(_notifications.size()), _notifications.isEmpty() ? nullptr : _notifications.last(), _parentWidget);

	notification->show();

	QTimer::singleShot(10, notification, &Notification::updatePosition);

	_notifications.append(notification);

	connect(notification, &Notification::finished, this, [this, notification]() {
		_notifications.removeOne(notification);
	});
}

void Notifications::setParentWidget(QWidget* parentWidget)
{
    _parentWidget = parentWidget;
}

}