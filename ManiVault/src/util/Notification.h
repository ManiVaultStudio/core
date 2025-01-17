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
    explicit Notification(const QString& message, QWidget* parent = nullptr);

    void showNotification(const QPoint& pos);
    void closeNotification();

    void onFadeOutFinished();

    bool isClosing() const;

signals:
    void finished(); // Signal emitted when the toaster finishes displaying

private:
    QLabel*         _label;
    QPushButton     _closePushButton;
    bool            _closing;           /** Whether this notification is being closed */
};

}