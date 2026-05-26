// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowConsoleFormatter.h"

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

QString WorkflowConsoleFormatter::format(SeverityLevel severity, const QString& text, const QString&, const QVariantMap& details)
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

    const int depth = details.value("depth").toInt();

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

}