// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowProgressNode.h"

namespace mv::workflow
{

class CORE_EXPORT WorkflowConsoleFormatter
{
public:
    static QString [[nodiscard]] format(SeverityLevel severity, const QString& text, const QString& location, const QVariantMap& details, std::uint32_t maxDepth = std::numeric_limits<std::uint32_t>::max());
    static QString [[nodiscard]] formatProgressTree(const WorkflowProgressNode::Snapshot& root);

private:
    static void appendProgressNode(QStringList& lines, const WorkflowProgressNode::Snapshot& node, int depth);
    static [[nodiscard]] QString statusName(WorkflowProgressNode::Status status);

    static [[nodiscard]] QString iconForEvent(const QString& event);
    static [[nodiscard]] QString labelForEntity(const QString& entity);
};

}
