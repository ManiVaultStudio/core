// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notification.h"

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QVBoxLayout>

namespace mv::util
{

Notification::Notification(const QString& message, QWidget* parent) :
	QWidget(parent),
	_label(new QLabel(message, this))
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	_label->setStyleSheet("QLabel { background-color: #333; color: white; padding: 10px; "
		"border-radius: 5px; font-size: 14px; }");
	_label->setAlignment(Qt::AlignCenter);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->addWidget(_label);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	// Fade-in animation
	QPropertyAnimation* fadeIn = new QPropertyAnimation(this, "windowOpacity");
	fadeIn->setDuration(500);
	fadeIn->setStartValue(0.0);
	fadeIn->setEndValue(1.0);
	fadeIn->start();

	// Auto-hide after 3 seconds
	QTimer::singleShot(3000, this, &Notification::hideToaster);
}

void Notification::showToaster(const QPoint& pos)
{
	adjustSize();
	move(pos);
	show();
}

void Notification::hideToaster()
{
	QPropertyAnimation* fadeOut = new QPropertyAnimation(this, "windowOpacity");

	fadeOut->setDuration(500);
	fadeOut->setStartValue(1.0);
	fadeOut->setEndValue(0.0);

	connect(fadeOut, &QPropertyAnimation::finished, this, &Notification::onFadeOutFinished);

	fadeOut->start();
}

void Notification::onFadeOutFinished()
{
	emit finished();
	deleteLater();
}

}