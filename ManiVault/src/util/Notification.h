// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWidget>

namespace mv::util
{

class CORE_EXPORT Notification : public QWidget
{

    Q_OBJECT

public:
    explicit Notification(const QString& title, const QString& description, const QIcon& icon, Notification* previousNotification, QWidget* parent = nullptr);

    void closeNotification();

    void onFadeOutFinished();

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

    void showEvent(QShowEvent* event) override;

    void updatePosition();

signals:
    void finished(); // Signal emitted when the toaster finishes displaying

private:
    QPointer<Notification>  _previousNotification;      /** Pointer to previous notification (maybe nullptr) */
    QPointer<Notification>  _nextNotification;          /** Pointer to next notification (maybe nullptr) */
    bool                    _closing;                   /** Whether this notification is being closed */

    static const int        notificationSpacing = 5;    /** Spacing between notifications */

    friend class Notifications;
};

}