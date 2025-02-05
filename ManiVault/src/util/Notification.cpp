// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notification.h"

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStatusBar>
#include <QPushButton>
#include <QUuid>

namespace mv::util
{

Notification::Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, QWidget* parent) :
	QWidget(parent),
    _previousNotification(previousNotification),
    _closing(false)
{
    setProperty("id", QUuid::createUuid());
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
    setAutoFillBackground(true);

    if (_previousNotification)
        _previousNotification->_nextNotification = this;

    if (_previousNotification) {
		connect(_previousNotification, &QObject::destroyed, this, &Notification::updatePosition);
    }

    auto mainLayout                 = new QVBoxLayout();
    auto notificationWidget         = new QWidget();
    auto notificationWidgetLayout   = new QHBoxLayout(this);
    auto iconLabel                  = new QLabel(this);
    auto messageLabel               = new QLabel(this);
    auto closePushButton            = new QPushButton(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);

    notificationWidget->setObjectName("Notification");
    notificationWidget->setStyleSheet("QWidget#Notification { border: 1px solid rgb(220, 220, 220); border-radius: 2px; }");
    notificationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    notificationWidget->setFixedWidth(300);

    iconLabel->setPixmap(icon.pixmap(24, 24));

    messageLabel->setWordWrap(true);
    messageLabel->setTextFormat(Qt::RichText);
    messageLabel->setText("<b>" + title + "</b>" + "<br>" + description);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    //messageLabel->setStyleSheet("QLabel { background-color: rgb(50, 50, 50); }");

    closePushButton->setFixedSize(24, 24);
    closePushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    closePushButton->setFlat(true);

    //notificationWidgetLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    notificationWidgetLayout->setContentsMargins(4, 4, 4, 4);
    notificationWidgetLayout->setSpacing(10);

    notificationWidgetLayout->addWidget(iconLabel);
	notificationWidgetLayout->addWidget(messageLabel, 1);
	notificationWidgetLayout->addWidget(closePushButton);

    notificationWidget->setLayout(notificationWidgetLayout);

    mainLayout->addWidget(notificationWidget);

    setLayout(mainLayout);

	auto fadeIn = new QPropertyAnimation(this, "windowOpacity");

	fadeIn->setDuration(300);
	fadeIn->setStartValue(0.0);
	fadeIn->setEndValue(1.0);
	fadeIn->start();

	QTimer::singleShot(5000, this, &Notification::closeNotification);

    connect(closePushButton, &QPushButton::clicked, this, &Notification::closeNotification);

	connect(this, &Notification::finished, this, [this]() -> void {
        if (getNextNotification())
            getNextNotification()->setPreviousNotification(getPreviousNotification());
    });

    qDebug() << "Notification" << property("id").toString() << getPreviousNotification();
}

void Notification::closeNotification()
{
    if (_closing)
        return;

    qDebug() << "Close notification" << property("id").toString() << height();

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

    if (_previousNotification && _nextNotification)
        _previousNotification->setNextNotification(_nextNotification);

    if (_nextNotification && _previousNotification)
        _nextNotification->setPreviousNotification(_previousNotification);

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

void Notification::setNextNotification(Notification* nextNotification)
{
    _nextNotification = nextNotification;

    updatePosition();
}

void Notification::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    qDebug() << "Notification show event" << property("id").toString() << height();

    QTimer::singleShot(10, this, &Notification::updatePosition);
}

void Notification::updatePosition()
{
    if (_previousNotification) {
        _previousNotification->updateGeometry();
        _previousNotification->adjustSize();

        move(QPoint(_previousNotification->pos().x(), _previousNotification->pos().y() - height() - notificationSpacing)); // 
    } else {
        move(parentWidget()->mapToGlobal(QPoint(notificationSpacing, Application::getMainWindow()->height() - Application::getMainWindow()->statusBar()->height() - height() - notificationSpacing)));
    }
        
    if (_nextNotification)
        QTimer::singleShot(25, _nextNotification, &Notification::updatePosition);
}

}
