// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notification.h"

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QHBoxLayout>

namespace mv::util
{

Notification::Notification(const QString& message, QWidget* parent) :
	QWidget(parent),
	_label(new QLabel(message, this)),
    _closing(false)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

    setFixedWidth(200);

	//_label->setStyleSheet("QLabel { background-color: #333; color: white; padding: 10px; "
	//	"border-radius: 5px; font-size: 14px; }");

    _label->setStyleSheet("QLabel { padding: 10px; border-radius: 5px; border: 1px solid grey; }");

	_label->setAlignment(Qt::AlignCenter);

    _closePushButton.setFixedSize(10, 10);
    _closePushButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));

	auto layout = new QHBoxLayout(this);

	layout->addWidget(_label);
	layout->addWidget(&_closePushButton);

	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);

	// Fade-in animation
	QPropertyAnimation* fadeIn = new QPropertyAnimation(this, "windowOpacity");

	fadeIn->setDuration(300);
	fadeIn->setStartValue(0.0);
	fadeIn->setEndValue(1.0);
	fadeIn->start();

	// Auto-hide after 3 seconds
	QTimer::singleShot(5000, this, &Notification::closeNotification);

    connect(&_closePushButton, &QPushButton::clicked, this, &Notification::closeNotification);
}

void Notification::showNotification(const QPoint& pos)
{
	adjustSize();
	show();
}

void Notification::closeNotification()
{
    _closing = true;

	auto fadeOut = new QPropertyAnimation(this, "windowOpacity");

	fadeOut->setDuration(300);
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

bool Notification::isClosing() const
{
    return _closing;
}

}
