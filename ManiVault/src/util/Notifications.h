// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "Notification.h"

#include <QObject>
#include <QPoint>
#include <QVector>

namespace mv::util
{

class CORE_EXPORT Notifications : public QObject
{
    Q_OBJECT

public:
    enum Position { BottomCenter, TopCenter, BottomRight };

    explicit Notifications(QWidget* parent = nullptr, Position position = BottomCenter);

    void showMessage(const QString& message);

    void setParentWidget(QWidget* parentWidget);

    std::int32_t getNumberOfActiveNotifications() const;

private:
    void positionNotification(Notification* notification) const;

    void adjustNotificationPositions();

    QWidget*                _parentWidget;
    Position                _position;
    QVector<Notification*>  _notifications;
};

}