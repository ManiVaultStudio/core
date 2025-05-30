// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Task.h"

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

protected:

    /** For custom drawing of the notification widget background */
    class NotificationWidget : public QWidget
    {
    public:

        /** No need for custom constructor*/
    	using QWidget::QWidget;

        /**
         * Override the default paint behavior
         * @param event Pointer to paint event that occurred
         */
        void paintEvent(QPaintEvent* event) override;

        QSize sizeHint() const override;
    };

public:

    /** Duration types */
    enum class DurationType {
        Fixed,          /** Standard 5000 ms */
        Calculated,     /** Calculated based on the number of characters */
        Task            /** Notification is visible as long as the task is running */
    };

public:

    /**
     * Construct a notification with \p title, \p description, \p icon, \p previousNotification and pointer to \p parent widget
     * @param title Notification title
     * @param description Notification description
     * @param icon Notification icon
     * @param previousNotification Pointer to previous notification (maybe nullptr)
     * @param durationType Duration type of the notification
     * @param parent Pointer to parent widget
     */
    explicit Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, const DurationType& durationType, QWidget* parent = nullptr);

    /**
     * Construct a notification with \p task, \p previousNotification and pointer to \p parent widget
     * @param task Task containing the notification details
     * @param previousNotification Pointer to previous notification (maybe nullptr)
     * @param parent Pointer to parent widget
     */
    explicit Notification(const Task& task, Notification* previousNotification, QWidget* parent = nullptr);

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

    /**
     * Get the notification title
     * @return Notification title
     */
    QString getTitle() const;

    /**
     * Set the notification title to \p title and update the message text label
     * @param title Notification title
     */
    void setTitle(const QString& title);

    /**
     * Get the notification description
     * @return Notification description
     */
    QString getDescription() const;

    /**
     * Set the notification description to \p description and update the message text label
     * @param description Notification description
     */
    void setDescription(const QString& description);

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

    /** Slide the notification in */
    void slideIn();

    /** Slide the notification out */
    void slideOut();

    /** Update the message label with the latest Notification#_title and Notification#_description */
    void updateMessageLabel();

    /**
     * Estimate the reading time of \p text
     * @param text Input text
     * @return Estimated reading time in milliseconds
     */
    static double getEstimatedReadingTime(const QString& text);

signals:

    /** Signal emitted when the toaster finishes displaying */
    void finished();

private:
    QString                 _title;                         /** Notification title (maybe HTML) */
    QString                 _description;                   /** Notification description (maybe HTML) */
    QPointer<Notification>  _previousNotification;          /** Pointer to previous notification (maybe nullptr) */
    QPointer<Notification>  _nextNotification;              /** Pointer to next notification (maybe nullptr) */
    bool                    _closing;                       /** Whether this notification is being closed */
    QLabel                  _messageLabel;                  /** Label for the message text */

    static const int    fixedWidth              = 400;      /** Width of the notification */
    static const int    spacing                 = 5;        /** Spacing between notifications */
    static const int    fixedDuration           = 5000;     /** Duration of notification display */
    static const int    animationDuration       = 300;      /** Duration of notification animation */
    static const int    averageReadingSpeedWPM  = 200;      /** Average reading speed in words per minute */

    friend class Notifications;
};

}