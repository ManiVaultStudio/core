// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "ManiVaultGlobals.h"

#include <QObject>
#include <QColor>
#include <QPixmap>

namespace mv::util {

class CORE_EXPORT ManiVaultException : public std::runtime_error {
public:
    ManiVaultException(
        QString category,
        WorkflowMessageType severity,
        QString scope,
        QString message
    )
        : std::runtime_error(message.toStdString())
        , category(std::move(category))
        , severity(severity)
        , scope(std::move(scope))
        , message(std::move(message))
    {
    }

    QString category;
    WorkflowMessageType severity;
    QString scope;
    QString message;
};

class ManiVaultException : public std::runtime_error {
public:
    ManifoldException(
        Severity severity,
        QString message,
        QString code,
        QString scope = {}
    )
        : std::runtime_error(message.toStdString())
        , severity(severity)
        , message(std::move(message))
        , code(std::move(code))
        , scope(std::move(scope))
    {
    }

    Severity severity;
    QString message;
    QString code;
    QString scope;
};

}
