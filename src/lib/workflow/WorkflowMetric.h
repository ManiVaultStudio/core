// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QList>
#include <QString>
#include <QVariant>
#include <QVariantMap>

namespace mv::workflow
{

/**
 * @brief Defines the stored metric value type.
 */
enum class WorkflowMetricValueType
{
    Integer,        /**< Integral metric value. */
    FloatingPoint   /**< Floating-point metric value. */
};

/**
 * @brief Named metric captured during workflow execution.
 *
 * Workflow metrics are used for reporting execution timings, counts, resource
 * usage, and other measurements produced while a workflow runs.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
struct CORE_EXPORT WorkflowMetric
{
    /**
     * @brief Formats a metric value for display.
     * @param metric Metric whose value is formatted.
     * @return Display-ready metric value.
     */
    [[nodiscard]] static QString formatMetricValue(const WorkflowMetric& metric);

    /**
     * @brief Formats a metric for plain-text notifications.
     * @param metric Metric to format.
     * @return Plain-text notification fragment.
     */
    [[nodiscard]] static QString toNotificationString(const WorkflowMetric& metric);

    /**
     * @brief Formats a metric for HTML notifications.
     * @param metric Metric to format.
     * @return HTML notification fragment.
     */
    [[nodiscard]] static QString toHtmlNotificationString(const WorkflowMetric& metric);

    /**
     * @brief Formats metrics as a plain-text notification summary.
     * @param metrics Metrics to include.
     * @param separator Separator inserted between metric fragments.
     * @return Plain-text notification summary.
     */
    [[nodiscard]] static QString toNotificationSummary(const QList<WorkflowMetric>& metrics, const QString& separator = "\n");

    /**
     * @brief Formats metrics as an HTML notification summary.
     * @param metrics Metrics to include.
     * @return HTML notification summary.
     */
    [[nodiscard]] static QString getWorkflowMetricsHtmlNotificationSummary(const QList<WorkflowMetric>& metrics);

    QString     _name;      /**< Metric name. */
    QString     _unit;      /**< Metric unit label. */
    QVariant    _value;     /**< Metric value. */
    QVariantMap _metadata;  /**< Additional metric metadata. */
};

}
