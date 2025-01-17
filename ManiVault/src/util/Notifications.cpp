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
	Notification* toaster = new Notification(message, _parentWidget);

	// Calculate position for the new toaster
	QPoint pos = calculateNotificationPosition(toaster);
	toaster->showToaster(pos);

	// Keep track of active toasters
	_activeNotifications.append(toaster);

	// Remove toaster from active list when finished
	connect(toaster, &Notification::finished, this, [this, toaster]() {
		_activeNotifications.removeOne(toaster);
		adjustNotificationPositions();
	});
}

QPoint Notifications::calculateNotificationPosition(Notification* notification)
{
	constexpr int margin = 10; // Space between toasters
	int           x, y;

	switch (_position) {
		case BottomCenter:
		{
			x = (_parentWidget->width() - notification->width()) / 2;
			y = _parentWidget->height() - (_activeNotifications.size() + 1) * (notification->height() + margin);
			break;
		}

		case TopCenter:
		{
			x = (_parentWidget->width() - notification->width()) / 2;
			y = (_activeNotifications.size()) * (notification->height() + margin);
			break;
		}

		case BottomRight:
		{
			x = _parentWidget->width() - notification->width() - margin;
			y = _parentWidget->height() - (_activeNotifications.size() + 1) * (notification->height() + margin);
			break;
		}
	}

	return { x, y };
}

void Notifications::adjustNotificationPositions()
{
	for (int i = 0; i < _activeNotifications.size(); ++i) {
		constexpr int margin = 10;

		int x, y;

		switch (_position) {
			case BottomCenter:
			{
				x = (_parentWidget->width() - _activeNotifications[i]->width()) / 2;
				y = _parentWidget->height() - (i + 1) * (_activeNotifications[i]->height() + margin);
				break;
			}

			case TopCenter:
			{
				x = (_parentWidget->width() - _activeNotifications[i]->width()) / 2;
				y = i * (_activeNotifications[i]->height() + margin);
				break;
			}

			case BottomRight:
			{
				x = _parentWidget->width() - _activeNotifications[i]->width() - margin;
				y = _parentWidget->height() - (i + 1) * (_activeNotifications[i]->height() + margin);
				break;
			}
		}

		_activeNotifications[i]->move(QPoint(x, y));
	}
}

}