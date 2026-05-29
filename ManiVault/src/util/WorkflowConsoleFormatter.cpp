// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleFormatter.h"
#include "WorkflowExecutionNodeType.h"

#include <QStringList>
#include <QtMath>

namespace mv::util
{

QString WorkflowConsoleFormatter::iconForEvent(const QString& event)
{
    if (event == "started")
        return ">";

    if (event == "finished")
        return "[OK]";

    if (event == "failed")
        return "[FAILED]";

    if (event == "skipped")
        return "[SKIPPED]";

    return "-";
}

QString WorkflowConsoleFormatter::labelForEntity(const QString& entity)
{
    if (entity == "workflow")
        return "Workflow";

    if (entity == "nested-workflow")
        return "Nested workflow";

    if (entity == "sequential-stage")
        return "Stage";

    if (entity == "parallel-stage")
        return "Parallel stage";

    if (entity == "job")
        return "Job";

    return "Item";
}

QString WorkflowConsoleFormatter::format(SeverityLevel severity, const QString& text, const QString&, const QVariantMap& details, std::uint32_t maxDepth /*= std::numeric_limits<std::uint32_t>::max()*/)
{
    const auto event = details.value("event").toString();
    const auto entity = details.value("entity").toString();

    if (event.isEmpty() || entity.isEmpty()) {
        switch (severity) {
        case SeverityLevel::Info:    return QString("[INFO] %1").arg(text);
        case SeverityLevel::Warning: return QString("[WARNING] %1").arg(text);
        case SeverityLevel::Error:
        case SeverityLevel::Fatal:   return QString("[ERROR] %1").arg(text);
        }

        return text;
    }

	const auto depth = static_cast<std::uint32_t>(details.value("depth").toInt());

    if (depth >= maxDepth)
        return {};

    const QString indent(depth * 2, QLatin1Char(' '));
    const QString icon = iconForEvent(event);
    const QString label = labelForEntity(entity);

    QString line = QString("%1%2 %3: %4")
        .arg(indent)
        .arg(icon)
        .arg(label)
        .arg(text);

    if (details.contains("durationMs")) {
        constexpr int durationColumn = 95;

        const auto duration = QString("%1 ms")
            .arg(details.value("durationMs").toULongLong(), 6);

        line = line.leftJustified(durationColumn, QLatin1Char(' ')) + duration;
    }

    if (event == "failed" && details.contains("error"))
        line += QString("  ERROR: %1").arg(details.value("error").toString());

    if (event == "skipped" && details.contains("reason"))
        line += QString("  SKIPPED: %1").arg(details.value("reason").toString());

    return line;
}

QString WorkflowConsoleFormatter::formatProgressTree(const WorkflowProgressNode::Snapshot& root)
{
    QStringList lines;

    lines << QStringLiteral("Kind              Name                                      Status      Progress  Children    Time");
    lines << QStringLiteral("------------------------------------------------------------------------------------------------");

    appendProgressNode(lines, root, 0);

    return lines.join(QLatin1Char('\n'));
}

void WorkflowConsoleFormatter::appendProgressNode(QStringList& lines, const WorkflowProgressNode::Snapshot& node, int depth)
{
    const QString indent(depth * 2, QLatin1Char(' '));

    const auto kind     = getWorkflowExecutionNodeTypeName(node.type);
    const auto status   = statusName(node.status);
    const auto progress = QStringLiteral("%1%").arg(qRound(node.progress * 100.0), 3);
    const auto children = QStringLiteral("%1/%2").arg(node.completedChildCount).arg(node.childCount);
    const auto time     = QStringLiteral("%1 ms").arg(node.elapsedMilliseconds);

    lines << QStringLiteral("%1%2 %3 %4 %5 %6")
        .arg(indent + kind, -18)
        .arg(node.name.left(40), -40)
        .arg(status, -10)
        .arg(progress, -8)
        .arg(children, -10)
        .arg(time);

    for (const auto& child : node.children)
        appendProgressNode(lines, child, depth + 1);
}

QString WorkflowConsoleFormatter::statusName(WorkflowProgressNode::Status status)
{
    switch (status) {
    case WorkflowProgressNode::Status::Pending:
        return QStringLiteral("Pending");

    case WorkflowProgressNode::Status::Running:
        return QStringLiteral("Running");

    case WorkflowProgressNode::Status::Completed:
        return QStringLiteral("Completed");

    case WorkflowProgressNode::Status::Failed:
        return QStringLiteral("Failed");

    case WorkflowProgressNode::Status::Skipped:
        return QStringLiteral("Skipped");
    }

    return QStringLiteral("Unknown");
}

}
