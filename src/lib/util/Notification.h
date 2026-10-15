// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Task.h"

#include "actions/TaskAction.h"

#include <QWidget>

namespace mv::util
{

/**
 * @brief Toast-style notification widget.
 *
 * Notification renders one transient message in a stacked notification area.
 * Notifications can either show a title, description, and icon for a fixed or
 * calculated duration, or stay visible while an associated Task is running.
 *
 * Instances are linked to previous and next notifications so stacked widgets can
 * reposition when one is shown or dismissed. Notifications owns the lifecycle of
 * these widgets.
 *
 * @note This class is intended for internal use by Notifications.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT Notification : public QWidget
{
    Q_OBJECT

protected:

    /**
     * @brief Inner widget used to paint the notification background.
     */
    class NotificationWidget : public QWidget
    {
    public:

        /** Inherit QWidget constructors. */
        using QWidget::QWidget;

        /**
         * @brief Paints the rounded notification background.
         * @param event Paint event.
         */
        void paintEvent(QPaintEvent* event) override;

        /**
         * @brief Returns the preferred inner notification size.
         * @return Size hint for the notification background widget.
         */
        [[nodiscard]] QSize sizeHint() const override;
    };

public:

    /** Duration policy for a notification. */
    enum class DurationType {
        Fixed,       /**< Uses the standard fixed display duration */
        Calculated,  /**< Estimates display duration from message length */
        Task         /**< Stays visible while the associated task is running */
    };

public:

    /**
     * @brief Constructs a message notification.
     * @param title Notification title.
     * @param description Notification description, optionally rich text.
     * @param icon Notification icon.
     * @param previousNotification Previous notification in the stack, or nullptr.
     * @param durationType Display duration policy.
     * @param parent Optional parent widget.
     */
    explicit Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, const DurationType& durationType, QWidget* parent = nullptr);

    /**
     * @brief Constructs a task notification.
     * @param task Task whose state is displayed by the notification.
     * @param previousNotification Previous notification in the stack, or nullptr.
     * @param parent Optional parent widget.
     */
    explicit Notification(QPointer<Task> task, Notification* previousNotification, QWidget* parent = nullptr);

    /**
     * @brief Returns whether the notification is closing.
     * @return True if the notification is currently closing.
     */
    [[nodiscard]] bool isClosing() const;

    /**
     * @brief Returns the previous notification in the stack.
     * @return Previous notification, or nullptr.
     */
    [[nodiscard]] Notification* getPreviousNotification() const;

    /**
     * @brief Sets the previous notification in the stack.
     * @param previousNotification Previous notification, or nullptr.
     */
    void setPreviousNotification(Notification* previousNotification);

    /**
     * @brief Returns the next notification in the stack.
     * @return Next notification, or nullptr.
     */
    [[nodiscard]] Notification* getNextNotification() const;

    /**
     * @brief Sets the next notification in the stack.
     * @param nextNotification Next notification, or nullptr.
     */
    void setNextNotification(Notification* nextNotification);

    /**
     * @brief Returns the notification icon.
     * @return Notification icon.
     */
    [[nodiscard]] QIcon getIcon() const;

    /**
     * @brief Sets the notification icon.
     * @param icon Notification icon.
     */
    void setIcon(const QIcon& icon);

    /**
     * @brief Returns the notification title.
     * @return Notification title.
     */
    [[nodiscard]] QString getTitle() const;

    /**
     * @brief Sets the notification title.
     * @param title Notification title.
     */
    void setTitle(const QString& title);

    /**
     * @brief Returns the notification description.
     * @return Notification description.
     */
    [[nodiscard]] QString getDescription() const;

    /**
     * @brief Sets the notification description.
     * @param description Notification description.
     */
    void setDescription(const QString& description);

    /**
     * @brief Returns the preferred minimum notification size.
     * @return Minimum size hint.
     */
    [[nodiscard]] QSize minimumSizeHint() const override;

    /**
     * @brief Returns the preferred notification size.
     * @return Size hint.
     */
    [[nodiscard]] QSize sizeHint() const override;

protected:
    
    /**
     * @brief Handles initial positioning and slide-in animation when shown.
     * @param event Show event.
     */
    void showEvent(QShowEvent* event) override;

    /** Updates this notification and following notifications in the stack. */
    void updatePosition();

private:

    /** Requests the notification to close. */
    void requestFinish();

    /** Performs final operations after fade-out animation and before deletion. */
    void finish();

    /** Starts the slide-in animation. */
    void slideIn();

    /** Starts the slide-out animation. */
    void slideOut();

    /** Updates the icon label from the current icon. */
    void updateIconLabel();

    /** Updates title and message labels from the current notification text. */
    void updateMessageLabel();

    /**
     * @brief Estimates reading time for notification text.
     * @param text Text to estimate.
     * @return Estimated reading time in milliseconds.
     */
    [[nodiscard]] static double getEstimatedReadingTime(const QString& text);

signals:

    /** Emitted when this notification has finished displaying. */
    void finished();

    /**
     * @brief Emitted when a hyperlink in the notification is clicked.
     * @param url Activated hyperlink URL.
     */
    void linkActivated(const QUrl& url);

private:
    QIcon                   _icon;                          /**< Notification icon, may be empty */
    QString                 _title;                         /**< Notification title, may contain rich text */
    QString                 _description;                   /**< Notification description, may contain rich text */
    QPointer<Task>          _task;                          /**< Task associated with this notification, or nullptr */
    QPointer<Notification>  _previousNotification;          /**< Previous notification in the stack, or nullptr */
    QPointer<Notification>  _nextNotification;              /**< Next notification in the stack, or nullptr */
    bool                    _closing;                       /**< Whether this notification is closing */
    QHBoxLayout             _notificationWidgetLayout;      /**< Layout of the notification widget */
    QVBoxLayout             _messageLayout;                 /**< Layout containing title, message, and task content */
    QLabel                  _iconLabel;                     /**< Label showing the notification icon */
    QLabel                  _titleLabel;                    /**< Label showing the notification title */
    QLabel                  _messageLabel;                  /**< Label showing the notification message */
    gui::TaskAction         _taskAction;                    /**< Task action shown for task notifications */

    static const int    margin                  = 9;        /**< Margin around the notification widget */
    static const int    fixedWidth              = 500;      /**< Fixed notification width in pixels */
    static const int    spacing                 = 5;        /**< Spacing between stacked notifications */
    static const int    fixedDuration           = 5000;     /**< Fixed display duration in milliseconds */
    static const int    animationDuration       = 300;      /**< Animation duration in milliseconds */
    static const int    averageReadingSpeedWPM  = 200;      /**< Average reading speed used for calculated duration */

    friend class Notifications;
};

}
