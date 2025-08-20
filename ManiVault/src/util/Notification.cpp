// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Notification.h"

#include "Application.h"

#include "StyledIcon.h"

#include "actions/ColorAction.h"

#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QHBoxLayout>
#include <qlayout.h>
#include <QMainWindow>
#include <QStatusBar>
#include <QPushButton>
#include <QPainter>
#include <QPainterPath>



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
        fixedWidth,
        0
	};
}

Notification::Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, const DurationType& durationType, QWidget* parent) :
	QWidget(parent),
    _icon(icon),
    _title(title),
	_description(description),
    _previousNotification(previousNotification),
    _closing(false),
    _taskAction(nullptr, "Task")
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);
    hide();

    if (_previousNotification)
        _previousNotification->_nextNotification = this;

    if (_previousNotification) {
		connect(_previousNotification, &QObject::destroyed, this, &Notification::updatePosition);
    }

    auto mainLayout                 = new QVBoxLayout();
    auto notificationWidget         = new NotificationWidget();
    auto closePushButton            = new QToolButton(this);

    mainLayout->setContentsMargins(0, 0, 0, 0);

    notificationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    notificationWidget->setFixedWidth(fixedWidth);
    notificationWidget->setMinimumHeight(10);
    notificationWidget->setAutoFillBackground(true);
    notificationWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    notificationWidget->setAttribute(Qt::WA_TranslucentBackground);

    _iconLabel.setStyleSheet("padding: 3px;");

    _messageLabel.setWordWrap(false);
    _messageLabel.setTextFormat(Qt::PlainText);

    _messageLabel.setWordWrap(true);
    _messageLabel.setMinimumHeight(10);
    _messageLabel.setOpenExternalLinks(true);

    closePushButton->setFixedSize(18, 18);
    closePushButton->setIcon(StyledIcon("xmark"));
    closePushButton->setAutoRaise(true);

    _messageLayout.setSpacing(4);

    _messageLayout.addWidget(&_titleLabel);
    _messageLayout.addWidget(&_messageLabel);

    _messageLayout.setAlignment(&_messageLabel, Qt::AlignTop);

    _notificationWidgetLayout.setContentsMargins(margin, margin, margin, margin);
    _notificationWidgetLayout.setSpacing(10);
    _notificationWidgetLayout.setAlignment(Qt::AlignTop);

    _notificationWidgetLayout.addWidget(&_iconLabel);
    _notificationWidgetLayout.setAlignment(&_iconLabel, Qt::AlignTop);

    _notificationWidgetLayout.addLayout(&_messageLayout, 1);

	_notificationWidgetLayout.addWidget(closePushButton);
    _notificationWidgetLayout.setAlignment(closePushButton, Qt::AlignTop);

    notificationWidget->setLayout(&_notificationWidgetLayout);

	updateIconLabel();
    updateMessageLabel();

    mainLayout->addWidget(notificationWidget);

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

Notification::Notification(QPointer<Task> task, Notification* previousNotification, QWidget* parent /*= nullptr*/) :
    Notification("", "", QIcon(), previousNotification, DurationType::Task, parent)
{
    if (!task) {
        qWarning() << "Notification task is null!";
        return;
    }

    _task = task;

	_taskAction.setTask(task);

    _messageLayout.addWidget(_taskAction.createWidget(this));

    setTitle(task->getName());

    const auto taskIcon = task->getIcon();

    setIcon(taskIcon.isNull() ? StyledIcon("stopwatch") : taskIcon);

    connect(task, &Task::nameChanged, this, &Notification::setTitle);
    connect(task, &Task::iconChanged, this, &Notification::setIcon);

    connect(task, &Task::statusChanged, this, [this](const Task::Status& previousStatus, const Task::Status& status) -> void {
        if (previousStatus == Task::Status::Running || previousStatus == Task::Status::RunningIndeterminate)
            QTimer::singleShot(1000, this, &Notification::requestFinish);
	});

    updateMessageLabel();
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

void Notification::updateIconLabel()
{
    _iconLabel.setPixmap(_icon.pixmap(32, 32));
}

void Notification::updateMessageLabel()
{
    _titleLabel.setText(QString("<b>%1</b>").arg(_title));

	_messageLabel.setText(_description);
    _messageLabel.setTextFormat(Qt::mightBeRichText(_description) ? Qt::RichText : Qt::PlainText);
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

QIcon Notification::getIcon() const
{
    return _icon;
}

void Notification::setIcon(const QIcon& icon)
{
	_icon = icon;

	updateIconLabel();
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

QSize Notification::minimumSizeHint() const
{
	return { fixedWidth, 10 };
}

QSize Notification::sizeHint() const
{
	return minimumSizeHint();
}

void Notification::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    QTimer::singleShot(10, this, [this]() -> void {
        adjustSize();
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
