// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowProgressNode.h"

namespace mv::util
{

class CORE_EXPORT WorkflowConsoleFormatter
{
public:
    static QString format(SeverityLevel severity, const QString& text, const QString& location, const QVariantMap& details);
    static QString formatProgressTree(const WorkflowProgressNode::Snapshot& root);

private:
    static void appendProgressNode(QStringList& lines, const WorkflowProgressNode::Snapshot& node, int depth);
    static QString statusName(WorkflowProgressNode::Status status);

    static QString iconForEvent(const QString& event);
    static QString labelForEntity(const QString& entity);
};

}
