// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Notification.h"

#include <QObject>

namespace mv::util
{
/**
 * Notifications manager class
 *
 * Orchestrates the creation and display of notifications (used solely by the help manager)
 *
 * Note: This class is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT Notifications : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct notifications manager with \p parent widget
     * @param parent Pointer to parent widget
     */
    explicit Notifications(QWidget* parent = nullptr);

    /**
     * Show message with \p title, \p description and \p icon
     * @param title Message title
     * @param description Message description
     * @param icon Message icon
     * @param durationType Duration type of the notification
     * @param delayMs Delay in milliseconds before the notification is shown
     */
    void showMessage(const QString& title, const QString& description, const QIcon& icon, const util::Notification::DurationType& durationType, std::int32_t delayMs);

    /**
     * Set parent widget to \p parentWidget (notification widgets will anchor to this widget)
     * @param parentWidget Pointer to parent widget
     */
    void setParentWidget(QWidget* parentWidget);

protected:

    /**
     * Watch \p watched for \p event
     * @param watched Pointer to watched object
     * @param event Event to watch
     * @return Whether the event was handled
     */
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QWidget*                _parentWidget;      /** Pointer to parent widget */
    QVector<Notification*>  _notifications;     /** Vector of notifications */
};

}