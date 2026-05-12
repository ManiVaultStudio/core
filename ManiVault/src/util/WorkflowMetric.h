// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>

namespace mv::util
{

enum class WorkflowMetricValueType
{
    Integer,
    FloatingPoint
};

struct CORE_EXPORT WorkflowMetric
{
    static QString formatMetricValue(const WorkflowMetric& metric);

    static QString toNotificationString(const WorkflowMetric& metric);

    static QString toHtmlNotificationString(const WorkflowMetric& metric);
    static QString toNotificationSummary(
        const QList<WorkflowMetric>& metrics,
        const QString& separator = "\n");

    static QString getWorkflowMetricsHtmlNotificationSummary(
        const QList<WorkflowMetric>& metrics);

    QString     _name;
    QString     _unit;
    QVariant    _value;
    QVariantMap _metadata;
};

} // namespace mv::util