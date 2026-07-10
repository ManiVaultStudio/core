// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QVariantMap>

namespace mv::workflow
{

/** Type stored by a workflow metric value. */
enum class WorkflowMetricValueType
{
    Integer,        /**< Integral metric value */
    FloatingPoint   /**< Floating-point metric value */
};

/**
 * @brief Named metric captured during workflow execution.
 */
struct CORE_EXPORT WorkflowMetric
{
    /** Formats a metric value for display. */
    static QString formatMetricValue(const WorkflowMetric& metric);

    /** Formats a metric for plain-text notifications. */
    static QString toNotificationString(const WorkflowMetric& metric);

    /** Formats a metric for HTML notifications. */
    static QString toHtmlNotificationString(const WorkflowMetric& metric);

    /** Formats multiple metrics as a plain-text notification summary. */
    static QString toNotificationSummary(
        const QList<WorkflowMetric>& metrics,
        const QString& separator = "\n");

    /** Formats multiple metrics as an HTML notification summary. */
    static QString getWorkflowMetricsHtmlNotificationSummary(
        const QList<WorkflowMetric>& metrics);

    QString     _name;      /**< Metric name */
    QString     _unit;      /**< Metric unit label */
    QVariant    _value;     /**< Metric value */
    QVariantMap _metadata;  /**< Additional metric metadata */
};

}
