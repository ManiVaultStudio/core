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

QString WorkflowConsoleFormatter::format(SeverityLevel, const QString& text, const QString&, const QVariantMap& details)
{
    const auto event = details.value("event").toString();
    const auto entity = details.value("entity").toString();

    if (event.isEmpty() || entity.isEmpty())
        return text;

    const int depth = details.value("depth").toInt();
    const QString indent(depth * 2, QLatin1Char(' '));

    QString line = QString("%1%2 %3: %4")
        .arg(indent)
        .arg(iconForEvent(event))
        .arg(labelForEntity(entity))
        .arg(text);

    if (details.contains("durationMs")) {
        line += QString("  %1 ms").arg(details.value("durationMs").toULongLong(), 8);
    }

    if (event == "skipped" && details.contains("reason")) {
        line += QString(" - %1").arg(details.value("reason").toString());
    }

    if (event == "failed" && details.contains("error")) {
        line += QString(" - %1").arg(details.value("error").toString());
    }

    if (event == "summary") {
        return QString("%1[OK] %2: %3  jobs: %4, ok: %5, failed: %6, skipped: %7, %8 ms")
            .arg(indent)
            .arg(labelForEntity(entity))
            .arg(text)
            .arg(details.value("jobsStarted").toInt())
            .arg(details.value("jobsFinished").toInt())
            .arg(details.value("jobsFailed").toInt())
            .arg(details.value("jobsSkipped").toInt())
            .arg(details.value("durationMs").toULongLong());
    }

    return line;
}

}