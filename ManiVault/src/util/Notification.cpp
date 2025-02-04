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

Notification::Notification(const QString& message, Notification* previousNotification, QWidget* parent) :
	QWidget(parent),
    _previousNotification(previousNotification),
	_label(new QLabel(message, this)),
    _closing(false)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
    setAutoFillBackground(true);

    setFixedWidth(200);

    if (_previousNotification)
        _previousNotification->_nextNotification = this;

    if (_previousNotification) {
        connect(_previousNotification, &QObject::destroyed, this, &Notification::closeNotification);
    }
        

	//_label->setStyleSheet("QLabel { background-color: #333; color: white; padding: 10px; "
	//	"border-radius: 5px; font-size: 14px; }");

    setStyleSheet("border: 1px solid grey;");

	_label->setAlignment(Qt::AlignCenter);

    _closePushButton.setFixedSize(10, 10);
    _closePushButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));

	auto layout = new QHBoxLayout(this);

	layout->addWidget(_label);
	//layout->addWidget(&_closePushButton);

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

	connect(this, &Notification::finished, this, [this]() -> void {
        qDebug() << "Notification finished";

        if (getNextNotification())
            getNextNotification()->setPreviousNotification(getPreviousNotification());
    });

    QTimer::singleShot(1, this, &Notification::updatePosition);
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

	fadeOut->setDuration(1500);
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

Notification* Notification::getPreviousNotification() const
{
    return _previousNotification;
}

void Notification::setPreviousNotification(Notification* previousNotification)
{
    _previousNotification = previousNotification;

    updatePosition();
}

Notification* Notification::getNextNotification() const
{
    return _nextNotification;
}

void Notification::updatePosition()
{
    if (_previousNotification)
        move(QPoint(_previousNotification->pos().x(), _previousNotification->pos().y() - _previousNotification->height() - 5));
    else
        move(parentWidget()->mapToGlobal(QPoint(0, parentWidget()->height() - height())));

    if (_nextNotification)
        QTimer::singleShot(1, _nextNotification, &Notification::updatePosition);
}

}
