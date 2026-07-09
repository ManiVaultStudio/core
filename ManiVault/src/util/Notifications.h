// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Notification.h"

#include <QObject>
#include <QUrl>

namespace mv::util
{
/**
 * @brief Manager for stacked toast notifications.
 *
 * Notifications creates Notification widgets, stacks them above the main window
 * status bar, repositions them when the parent window changes size, and forwards
 * activated notification links to interested consumers.
 *
 * @note This class is intended for internal use by the help manager.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT Notifications : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Constructs a notifications manager.
     * @param parent Parent widget used as the notification anchor.
     */
    explicit Notifications(QWidget* parent = nullptr);

    /**
     * @brief Shows a message notification.
     * @param title Notification title.
     * @param description Notification description, optionally rich text.
     * @param icon Notification icon.
     * @param durationType Display duration policy.
     * @param delayMs Delay in milliseconds before the notification is shown.
     */
    void showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs);

    /**
     * @brief Shows a task notification.
     * @param task Task whose state is displayed by the notification.
     */
    void showTask(QPointer<Task> task);

    /**
     * @brief Sets up synchronization with the main window.
     */
    void setupMainWindowSynchronization();

protected:

    /**
     * @brief Repositions notifications when watched widgets resize.
     * @param watched Watched object.
     * @param event Event to handle.
     * @return True if the event was handled.
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

private:

    /**
     * @brief Adds and displays a notification.
     * @param notification Notification to add.
     */
    void addNotification(Notification* notification);

    /**
     * @brief Updates the positions of all visible notifications.
     */
    void updateAllPositions();

signals:

    /**
     * @brief Emitted when a notification hyperlink is activated.
     * @param url Activated hyperlink URL.
     */
    void notificationLinkActivated(const QUrl& url);

private:
    QVector<Notification*>  _notifications;     /**< Currently visible notifications in stack order */
};

}
