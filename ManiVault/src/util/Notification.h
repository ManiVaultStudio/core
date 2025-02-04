// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QWidget>
#include <QPushButton>

namespace mv::util
{

class CORE_EXPORT Notification : public QWidget
{

    Q_OBJECT

public:
    explicit Notification(const QString& message, Notification* previousNotification, QWidget* parent = nullptr);

    void showNotification(const QPoint& pos);
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

protected:

    void updatePosition();

signals:
    void finished(); // Signal emitted when the toaster finishes displaying

private:
    QPointer<Notification>  _previousNotification;      /** Pointer to previous notification (maybe nullptr) */
    QPointer<Notification>  _nextNotification;          /** Pointer to next notification (maybe nullptr) */
    QLabel* _label;                     /** Pointer to label */ 
    QPushButton             _closePushButton;
    bool                    _closing;           /** Whether this notification is being closed */

    friend class Notifications;
};

}