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

Notification::Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, const DurationType& durationType, QWidget* parent) :
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

    const auto windowColorName = QApplication::palette().color(QPalette::ColorGroup::Normal, QPalette::Window).name();
    const auto borderColorName = QApplication::palette().color(QPalette::ColorGroup::Normal, QPalette::Mid).name();

    notificationWidget->setObjectName("Notification");
    notificationWidget->setStyleSheet(QString("QWidget#Notification { background-color: %1; border: 1px solid %2; border-radius: 5px; }").arg(windowColorName, borderColorName));
    notificationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    notificationWidget->setFixedWidth(fixedWidth);
    notificationWidget->setMinimumHeight(10);

    iconLabel->setStyleSheet("padding: 3px;");
    iconLabel->setPixmap(icon.pixmap(32, 32));

    messageLabel->setWordWrap(true);
    messageLabel->setTextFormat(Qt::RichText);
    messageLabel->setText("<b>" + title + "</b>" + "<br>" + description);
    messageLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    messageLabel->setMinimumHeight(10);
    messageLabel->setOpenExternalLinks(true);

    closePushButton->setFixedSize(16, 16);
    closePushButton->setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    closePushButton->setAutoRaise(true);

    notificationWidgetLayout->setContentsMargins(10, 10, 10, 10);
    notificationWidgetLayout->setSpacing(20);
    notificationWidgetLayout->setAlignment(Qt::AlignTop);

    notificationWidgetLayout->addWidget(iconLabel);
    notificationWidgetLayout->setAlignment(iconLabel, Qt::AlignTop);
    notificationWidgetLayout->addWidget(messageLabel, 1);
    notificationWidgetLayout->setAlignment(messageLabel, Qt::AlignTop);
    notificationWidgetLayout->addWidget(closePushButton);
    notificationWidgetLayout->setAlignment(closePushButton, Qt::AlignTop);

    notificationWidget->setLayout(notificationWidgetLayout);

    mainLayout->addWidget(notificationWidget);

    setLayout(mainLayout);

    const auto duration = durationType == DurationType::Fixed ? fixedDuration : getEstimatedReadingTime(title + description);

	QTimer::singleShot(duration, this, &Notification::requestFinish);

    connect(closePushButton, &QPushButton::clicked, this, &Notification::requestFinish);
}

void Notification::requestFinish()
{
    if (_closing)
        return;

    _closing = true;

    slideOut();
}

void Notification::finish()
{
    if (getPreviousNotification() && getNextNotification())
        getPreviousNotification()->setNextNotification(getNextNotification());

    if (getNextNotification())
        getNextNotification()->setPreviousNotification(getPreviousNotification());

    emit finished();
}

void Notification::slideIn()
{
    auto animationGroup         = new QParallelAnimationGroup(this);
    auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    auto positionAnimation      = new QPropertyAnimation(this, "pos");

    animationGroup->addAnimation(windowOpacityAnimation);
    animationGroup->addAnimation(positionAnimation);

    windowOpacityAnimation->setDuration(animationDuration);
    windowOpacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    windowOpacityAnimation->setStartValue(0.0);
    windowOpacityAnimation->setEndValue(1.0);

    positionAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    positionAnimation->setDuration(animationDuration);
    positionAnimation->setStartValue(pos() - QPoint(25, 0));
    positionAnimation->setEndValue(pos());

    animationGroup->start();
}

void Notification::slideOut()
{
    auto animationGroup = new QParallelAnimationGroup(this);
    auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    auto positionAnimation = new QPropertyAnimation(this, "pos");

    animationGroup->addAnimation(windowOpacityAnimation);
    animationGroup->addAnimation(positionAnimation);

    windowOpacityAnimation->setDuration(animationDuration);
    windowOpacityAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    windowOpacityAnimation->setStartValue(1.0);
    windowOpacityAnimation->setEndValue(0.0);

    positionAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    positionAnimation->setDuration(animationDuration);
    positionAnimation->setStartValue(pos());
    positionAnimation->setEndValue(pos() - QPoint(25, 0));

    connect(animationGroup, &QPropertyAnimation::finished, this, &Notification::finish);

    animationGroup->start();
}

double Notification::getEstimatedReadingTime(const QString& text)
{
    QRegularExpression wordRegex("\\b\\w+\\b");

	auto wordIterator = wordRegex.globalMatch(text);

	size_t wordCount = 0;

    while (wordIterator.hasNext()) {
        wordIterator.next();
        wordCount++;
    }

    const double readingTimeMinutes       = static_cast<float>(wordCount) / averageReadingSpeedWPM;
    const double readingTimeMilliseconds  = readingTimeMinutes * 60 * 1000;

    return readingTimeMilliseconds;
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

    QTimer::singleShot(10, this, [this]() -> void {
        updatePosition();
        slideIn();
    });
}

void Notification::updatePosition()
{
    if (_previousNotification) {
        _previousNotification->updateGeometry();
        _previousNotification->adjustSize();

        move(QPoint(_previousNotification->pos().x(), _previousNotification->pos().y() - height() - spacing));
    } else {
        const auto statusBarHeight = Application::getMainWindow()->statusBar()->isVisible() ? Application::getMainWindow()->statusBar()->height() : 0;

        move(parentWidget()->mapToGlobal(QPoint(spacing, Application::getMainWindow()->height() - statusBarHeight - height() - spacing)));
    }
        
    if (_nextNotification)
        QTimer::singleShot(25, _nextNotification, &Notification::updatePosition);
}

}
