// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QVariantMap>

namespace mv::workflow
{

/**
 * @brief Describes the numeric value type of a workflow metric.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
enum class WorkflowMetricValueType
{
    Integer,        /**< Integral metric value */
    FloatingPoint   /**< Floating-point metric value */
};

/**
 * @brief Represents a metric produced during workflow execution.
 *
 * WorkflowMetric stores a named measurement together with its value, unit, and
 * optional structured metadata. Metrics are typically used to summarize workflow
 * execution characteristics such as duration, counts, throughput, processed data
 * size, or other diagnostic values.
 *
 * Metrics can be formatted for plain-text and HTML notifications using the
 * provided helper functions.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMetric
{
    /**
     * @brief Formats the value of a workflow metric.
     *
     * @param metric Metric whose value should be formatted.
     * @return Human-readable metric value.
     */
    [[nodiscard]] static QString formatMetricValue(const WorkflowMetric& metric);

    /**
     * @brief Converts a workflow metric to a plain-text notification string.
     *
     * @param metric Metric to format.
     * @return Plain-text notification representation.
     */
    [[nodiscard]] static QString toNotificationString(const WorkflowMetric& metric);

    /**
     * @brief Converts a workflow metric to an HTML notification string.
     *
     * @param metric Metric to format.
     * @return HTML notification representation.
     */
    [[nodiscard]] static QString toHtmlNotificationString(const WorkflowMetric& metric);

    /**
     * @brief Creates a plain-text notification summary for multiple metrics.
     *
     * @param metrics Metrics to include in the summary.
     * @param separator Separator inserted between formatted metrics.
     * @return Plain-text notification summary.
     */
    [[nodiscard]] static QString toNotificationSummary(const QList<WorkflowMetric>& metrics, const QString& separator = "\n");

    /**
     * @brief Creates an HTML notification summary for multiple workflow metrics.
     *
     * @param metrics Metrics to include in the summary.
     * @return HTML notification summary.
     */
    [[nodiscard]] static QString getWorkflowMetricsHtmlNotificationSummary(const QList<WorkflowMetric>& metrics);

    QString     name;       /**< Human-readable metric name */
    QString     unit;       /**< Optional unit associated with the metric value */
    QVariant    value;      /**< Metric value */
    QVariantMap metadata;   /**< Optional structured metadata associated with the metric */
};

}
