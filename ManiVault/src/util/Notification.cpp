// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notification.h"

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QStatusBar>
#include <QPushButton>

#include "actions/ColorAction.h"

namespace mv::util
{

Notification::Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, QWidget* parent) :
	QWidget(parent),
    _previousNotification(previousNotification),
    _closing(false)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
    setAutoFillBackground(true);
    setMinimumHeight(10);

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
    auto closePushButton            = new QToolButton(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);

    const auto borderColorName = QApplication::palette().color(QPalette::ColorGroup::Normal, QPalette::Mid).name();

    notificationWidget->setObjectName("Notification");
    notificationWidget->setStyleSheet(QString("QWidget#Notification { border: 1px solid %1; border-radius: 2px; }").arg(borderColorName));
    notificationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    notificationWidget->setFixedWidth(notificationWidth);
    notificationWidget->setMinimumHeight(10);

    iconLabel->setPixmap(icon.pixmap(24, 24));

    messageLabel->setWordWrap(true);
    messageLabel->setTextFormat(Qt::RichText);
    messageLabel->setText("<b>" + title + "</b>" + "<br>" + description);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    messageLabel->setMinimumHeight(10);

    closePushButton->setFixedSize(24, 24);
    closePushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    closePushButton->setAutoRaise(true);

    notificationWidgetLayout->setContentsMargins(5, 5, 5, 5);
    notificationWidgetLayout->setSpacing(10);
    notificationWidgetLayout->setAlignment(Qt::AlignTop);

    notificationWidgetLayout->addWidget(iconLabel);
    notificationWidgetLayout->setAlignment(iconLabel, Qt::AlignCenter);
    notificationWidgetLayout->addWidget(messageLabel, 1);
    notificationWidgetLayout->setAlignment(messageLabel, Qt::AlignTop);
    notificationWidgetLayout->addWidget(closePushButton);
    notificationWidgetLayout->setAlignment(closePushButton, Qt::AlignTop);

    notificationWidget->setLayout(notificationWidgetLayout);

    mainLayout->addWidget(notificationWidget);

    setLayout(mainLayout);

	auto fadeInAnimation = new QPropertyAnimation(this, "windowOpacity");

	fadeInAnimation->setDuration(notificationAnimationDuration / 2);
	fadeInAnimation->setStartValue(0.0);
	fadeInAnimation->setEndValue(1.0);
	fadeInAnimation->start();

	QTimer::singleShot(notificationDuration, this, &Notification::requestFinish);

    connect(closePushButton, &QPushButton::clicked, this, &Notification::requestFinish);
}

void Notification::requestFinish()
{
    if (_closing)
        return;

    _closing = true;

    auto animationGroup         = new QParallelAnimationGroup(this);
	auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    auto positionAnimation      = new QPropertyAnimation(this, "pos");

    animationGroup->addAnimation(windowOpacityAnimation);
    animationGroup->addAnimation(positionAnimation);

	windowOpacityAnimation->setDuration(notificationAnimationDuration);
	windowOpacityAnimation->setStartValue(1.0);
	windowOpacityAnimation->setEndValue(0.0);

    positionAnimation->setEasingCurve(QEasingCurve::InQuad);
    positionAnimation->setDuration(notificationAnimationDuration);
    positionAnimation->setStartValue(pos());
    positionAnimation->setEndValue(pos() - QPoint(50, 0));

	connect(animationGroup, &QPropertyAnimation::finished, this, &Notification::finish);

    animationGroup->start();
}

void Notification::finish()
{
    if (getPreviousNotification() && getNextNotification())
        getPreviousNotification()->setNextNotification(getNextNotification());

    if (getNextNotification())
        getNextNotification()->setPreviousNotification(getPreviousNotification());

    emit finished();
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

    QTimer::singleShot(10, this, &Notification::updatePosition);
}

void Notification::updatePosition()
{
    if (_previousNotification) {
        _previousNotification->updateGeometry();
        _previousNotification->adjustSize();

        move(QPoint(_previousNotification->pos().x(), _previousNotification->pos().y() - height() - notificationSpacing));
    } else {
        move(parentWidget()->mapToGlobal(QPoint(notificationSpacing, Application::getMainWindow()->height() - Application::getMainWindow()->statusBar()->height() - height() - notificationSpacing)));
    }
        
    if (_nextNotification)
        QTimer::singleShot(25, _nextNotification, &Notification::updatePosition);
}

}
