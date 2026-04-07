// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

enum class OperationSeverity
{
    Warning,
    Error,
    Fatal
};

struct CORE_EXPORT OperationIssue
{
    OperationSeverity severity;
    QString scope;
    QString message;
};

class CORE_EXPORT OperationContext
{
public:
    void reportWarning(QString scope, QString message);

    void reportError(QString scope, QString message);

    void reportFatal(QString scope, QString message);

    bool hasWarnings() const;

    bool hasErrors() const;

    bool hasFatalError() const;

    QList<OperationIssue> issues() const;

    QString firstErrorMessage() const;

    QString combinedErrorMessage() const;

    void requestAbort();

    bool isAbortRequested() const;

private:
    void addIssue(OperationSeverity severity, QString scope, QString message);

    bool hasSeverity(OperationSeverity severity) const;

private:
    mutable QMutex          _mutex;
    QList<OperationIssue>   _issues;
    std::atomic_bool        _abortRequested = false;
};

}
