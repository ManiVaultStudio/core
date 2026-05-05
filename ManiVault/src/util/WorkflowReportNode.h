// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMessage.h"

#include <QString>
#include <QDateTime>
#include <QMutex>

namespace mv::util
{

class CORE_EXPORT WorkflowReportNode
{
public:
    using Ptr = std::shared_ptr<WorkflowReportNode>;

    explicit WorkflowReportNode(const QString& name);

    Ptr createChild(const QString& name);

    void addMessage(SeverityLevel level, const QString& source, const QString& text, const QString& scope, const QVariantMap& details = {});

    QString getName() const;

    QVector<WorkflowMessage> getMessages() const;

    QVector<Ptr> getChildren() const;

    bool hasErrorsRecursive() const;

    int getWarningCountRecursive() const;

    int getErrorCountRecursive() const;

private:
    QString _name;
    mutable QMutex _mutex;
    QVector<WorkflowMessage> _messages;
    QVector<Ptr> _children;
};

} // namespace mv::util