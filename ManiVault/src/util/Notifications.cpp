// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#include "Notifications.h"

#include "Application.h"

#include <QApplication>
#include <QMainWindow>
#include <QEvent>
#include <QTimer>
#include <QWindow>

namespace mv::util
{

Notifications::Notifications(QWidget* parent) :
    QObject(parent)
{
}

void Notifications::showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs)
{
    if (Application::getMainWindow()) {
        const auto createNotification = [this, title, description, icon, durationType]() -> Notification* {
            return new Notification(title, description, icon, _notifications.isEmpty() ? nullptr : _notifications.last(), durationType, nullptr);
        };

        if (delayMs > 0) {
            QTimer::singleShot(delayMs, this, [this, createNotification]() {
                addNotification(createNotification());
            });
        }
        else {
            addNotification(createNotification());
        }
    }
}

void Notifications::showTask(QPointer<Task> task)
{
    if (Application::getMainWindow()) {
        auto notification = new Notification(task, _notifications.isEmpty() ? nullptr : _notifications.last(), nullptr);
        addNotification(notification);
    }
}

void Notifications::setupMainWindowSynchronization()
{
    if (auto mainWindow = Application::getMainWindow()) {
        mainWindow->removeEventFilter(this);
        mainWindow->installEventFilter(this);

        connect(qApp, &QApplication::focusWindowChanged, this, [this](QWindow*) {
            updateTransientParents();
        }, Qt::UniqueConnection);

        updateTransientParents();
    }
}

bool Notifications::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::Resize) {
        QTimer::singleShot(10, this, &Notifications::updateAllPositions);
    }

    return QObject::eventFilter(watched, event);
}

void Notifications::addNotification(Notification* notification)
{
    Q_ASSERT(notification);

    if (!notification)
        return;

    connect(notification, &Notification::linkActivated, this, &Notifications::notificationLinkActivated);
    connect(notification, &Notification::finished, this, [this, notification]() {
        _notifications.removeOne(notification);
        notification->deleteLater();

        updateAllPositions();
    });

    _notifications.append(notification);
    updateTransientParents();
    notification->updatePosition();
    notification->show();
}

void Notifications::updateAllPositions()
{
    updateTransientParents();

    for (auto repositionNotification : _notifications)
        repositionNotification->updatePosition();
}

void Notifications::updateTransientParents()
{
    auto* mainWindow = Application::getMainWindow();
    auto* transientParent = QApplication::activeWindow();
    auto* mainWindowHandle = mainWindow ? mainWindow->windowHandle() : nullptr;

    if (!transientParent || !transientParent->windowHandle())
        transientParent = mainWindow;

    auto* transientParentHandle = transientParent ? transientParent->windowHandle() : nullptr;

    if (transientParentHandle) {
        for (auto notification : _notifications) {
            if (notification->windowHandle() == transientParentHandle) {
                transientParentHandle = mainWindowHandle;
                break;
            }
        }
    }

    for (auto notification : _notifications) {
        notification->winId();

        if (auto* notificationWindow = notification->windowHandle()) {
            if (notificationWindow->transientParent() != transientParentHandle)
                notificationWindow->setTransientParent(transientParentHandle);
        }
    }

    if (transientParentHandle && _trackedTransientParent != transientParentHandle) {
        _trackedTransientParent = transientParentHandle;
        auto* trackedTransientParent = transientParentHandle;

        connect(trackedTransientParent, &QObject::destroyed, this, [this, trackedTransientParent]() {
            if (_trackedTransientParent == trackedTransientParent) {
                _trackedTransientParent = nullptr;
                updateTransientParents();
            }
        });

        connect(trackedTransientParent, &QWindow::visibilityChanged, this, [this, trackedTransientParent](QWindow::Visibility visibility) {
            if (_trackedTransientParent == trackedTransientParent && visibility == QWindow::Hidden)
                updateTransientParents();
        });
    }
}

}
