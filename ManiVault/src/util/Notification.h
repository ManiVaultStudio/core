// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWidget>

namespace mv::util
{

/**
 * Notification widget class
 *
 * For showing stacked notifications in a toaster-like way (see the notifications class).
 *
 * Note: This class is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Notification : public QWidget
{

    Q_OBJECT

public:

    /**
     * Construct a notification with \p title, \p description, \p icon, \p previousNotification and pointer to \p parent widget
     * @param title Notification title
     * @param description Notification description
     * @param icon Notification icon
     * @param previousNotification Pointer to previous notification (maybe nullptr)
     * @param parent Pointer to parent widget
     */
    explicit Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, QWidget* parent = nullptr);

    /**
     * Get whether the notification is closing
     * @return Boolean determining Whether the notification is closing or not
     */
    bool isClosing() const;

    /**
     * Get previous notification
     * @return Pointer to previous notification (maybe nullptr)
     */
    Notification* getPreviousNotification() const;

    /**
     * Set previous notification to \p previousNotification
     * @param previousNotification Pointer to previous notification (maybe nullptr)
     */
    void setPreviousNotification(Notification* previousNotification);

    /**
     * Get next notification
     * @return Pointer to next notification (maybe nullptr)
     */
    Notification* getNextNotification() const;

    /**
     * Set next notification to \p nextNotification
     * @param nextNotification Pointer to next notification (maybe nullptr)
     */
    void setNextNotification(Notification* nextNotification);

protected:
    
    /**
     * Invoked when the widget is shown (used to update the position at the correct time)
     * @param event Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

    /** Update the position of the notification and other notifications after it */
    void updatePosition();

private:

    /** Request the notification to finish */
    void requestFinish();

    /** Perform final operations after fade-out animation and before being deleted */
	void finish();

signals:

    /** Signal emitted when the toaster finishes displaying */
    void finished();

private:
    QPointer<Notification>  _previousNotification;                      /** Pointer to previous notification (maybe nullptr) */
    QPointer<Notification>  _nextNotification;                          /** Pointer to next notification (maybe nullptr) */
    bool                    _closing;                                   /** Whether this notification is being closed */

    static const int        notificationWidth               = 350;      /** Width of the notification */
    static const int        notificationSpacing             = 5;        /** Spacing between notifications */
    static const int        notificationDuration            = 5000;     /** Duration of notification display */
    static const int        notificationAnimationDuration   = 200;      /** Duration of notification animation */

    friend class Notifications;
};

}