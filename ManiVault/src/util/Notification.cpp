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
#include <QPainterPath>

#include "actions/ColorAction.h"

namespace mv::util
{

void Notification::NotificationWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

	painter.setRenderHint(QPainter::Antialiasing);

    const auto windowColor = QApplication::palette().color(QPalette::ColorGroup::Normal, QPalette::Window);
    const auto borderColor = QApplication::palette().color(QPalette::ColorGroup::Normal, QPalette::Light);

    constexpr int radius = 10;

    QPainterPath path;

	path.addRoundedRect(rect().adjusted(1, 1, -1, -1), radius, radius);

    painter.fillPath(path, windowColor);
    painter.setPen(QPen(borderColor, 1));
    painter.drawPath(path);
}

QSize Notification::NotificationWidget::sizeHint() const
{
	return {
        400,
        0
	};
}

Notification::Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, const DurationType& durationType, QWidget* parent) :
	QWidget(parent),
    _title(title),
    _description(description),
    _previousNotification(previousNotification),
    _closing(false)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
    setMinimumHeight(10);

    if (_previousNotification)
        _previousNotification->_nextNotification = this;

    if (_previousNotification) {
		connect(_previousNotification, &QObject::destroyed, this, &Notification::updatePosition);
    }

    auto mainLayout                 = new QVBoxLayout();
    auto notificationWidget         = new NotificationWidget();
    auto notificationWidgetLayout   = new QHBoxLayout(this);
    auto iconLabel                  = new QLabel(this);
    auto closePushButton            = new QToolButton(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);

    notificationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    notificationWidget->setFixedWidth(fixedWidth);
    notificationWidget->setMinimumHeight(10);
    notificationWidget->setAutoFillBackground(true);
    notificationWidget->setAttribute(Qt::WA_TranslucentBackground);

    iconLabel->setStyleSheet("padding: 3px;");
    iconLabel->setPixmap(icon.pixmap(32, 32));
    iconLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    _messageLabel.setWordWrap(true);
    _messageLabel.setTextFormat(Qt::RichText);
    _messageLabel.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    _messageLabel.setMinimumHeight(10);
    _messageLabel.setOpenExternalLinks(true);

    closePushButton->setFixedSize(18, 18);
    closePushButton->setIcon(StyledIcon("xmark"));
    closePushButton->setAutoRaise(true);

    notificationWidgetLayout->setContentsMargins(10, 10, 10, 10);
    notificationWidgetLayout->setSpacing(10);
    notificationWidgetLayout->setAlignment(Qt::AlignTop);

    notificationWidgetLayout->addWidget(iconLabel);
    notificationWidgetLayout->setAlignment(iconLabel, Qt::AlignTop);
    notificationWidgetLayout->addWidget(&_messageLabel, 1);
    notificationWidgetLayout->setAlignment(&_messageLabel, Qt::AlignTop);
    notificationWidgetLayout->addWidget(closePushButton);
    notificationWidgetLayout->setAlignment(closePushButton, Qt::AlignTop);

    notificationWidget->setLayout(notificationWidgetLayout);

    updateMessageLabel();

    mainLayout->addWidget(notificationWidget);

    qApp->processEvents();

    notificationWidget->adjustSize();

    setLayout(mainLayout);

	switch (durationType) {
	    case DurationType::Fixed:
	    case DurationType::Calculated:
	    {
            const auto duration = durationType == DurationType::Fixed ? fixedDuration : getEstimatedReadingTime(title + description);

            QTimer::singleShot(duration, this, &Notification::requestFinish);

	    	break;
	    }

        case DurationType::Task:
            break;
	}
    
    connect(closePushButton, &QPushButton::clicked, this, &Notification::requestFinish);
}

Notification::Notification(const Task& task, Notification* previousNotification, QWidget* parent /*= nullptr*/) :
    Notification(task.getName(), task.getDescription(), task.getIcon(), previousNotification, DurationType::Task, parent)
{
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
    auto animationGroup         = new QParallelAnimationGroup(this);
    auto windowOpacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    auto positionAnimation      = new QPropertyAnimation(this, "pos");

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

void Notification::updateMessageLabel()
{
    qDebug() << "Updating message text label with title: " << _title << " and description: " << _description;
    _messageLabel.setText("<b>" + _title + "</b>" + "<br>" + _description);
    _messageLabel.adjustSize();
}

double Notification::getEstimatedReadingTime(const QString& text)
{
    QRegularExpression wordRegex(R"(\b\w+\b)");

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

QString Notification::getTitle() const
{
	return _title;
}

void Notification::setTitle(const QString& title)
{
    if (_title == title)
        return;

	_title = title;

	updateMessageLabel();
}

QString Notification::getDescription() const
{
	return _description;
}

void Notification::setDescription(const QString& description)
{
    if (_description == description)
        return;

	_description = description;

	updateMessageLabel();
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

        move(QPoint(parentWidget()->mapToGlobal(QPoint(spacing, 0)).x(), _previousNotification->pos().y() - height() - spacing));
    } else {
        const auto statusBarHeight = Application::getMainWindow()->statusBar()->isVisible() ? Application::getMainWindow()->statusBar()->height() : 0;

        move(parentWidget()->mapToGlobal(QPoint(spacing, Application::getMainWindow()->height() - statusBarHeight - height() - spacing)));
    }
        
    if (_nextNotification)
        QTimer::singleShot(25, _nextNotification, &Notification::updatePosition);
}

}
