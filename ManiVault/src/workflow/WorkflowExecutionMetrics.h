// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMetric.h"

#include <QString>

#include <mutex>

namespace mv::workflow
{

/**
 * @brief Thread-safe collection of execution metrics.
 *
 * WorkflowExecutionMetrics stores named integer and double counters that can be
 * updated while a workflow is running. Registered metrics retain unit and
 * metadata information, and snapshot() converts the current atomic counter
 * values into WorkflowMetric objects for reporting.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionMetrics
{
public:

    /**
     * @brief Registers an integer metric.
     * @param name Metric name.
     * @param unit Unit label for the metric value.
     * @param metadata Optional structured metric metadata.
     */
    void registerInteger(const QString& name, const QString& unit, QVariantMap metadata = {});

    /**
     * @brief Registers a double metric.
     * @param name Metric name.
     * @param unit Unit label for the metric value.
     * @param metadata Optional structured metric metadata.
     */
    void registerDouble(const QString& name, const QString& unit, QVariantMap metadata = {});

    /**
     * @brief Adds to an integer metric value.
     * @param name Metric name.
     * @param amount Amount to add.
     */
    void addInteger(const QString& name, std::uint64_t amount);

    /**
     * @brief Adds to a double metric value.
     * @param name Metric name.
     * @param amount Amount to add.
     */
    void addDouble(const QString& name, double amount);

    /**
     * @brief Returns a snapshot of all registered metrics.
     * @return Current metric values as workflow metrics.
     */
    [[nodiscard]]
    QVector<WorkflowMetric> snapshot() const;

private:
    /**
     * @brief Registered metric with atomically updated storage.
     */
    struct AtomicMetric
    {
        QString                     name;               /**< Metric name */
        QString                     unit;               /**< Unit label for the metric value */
        QVariantMap                 metadata;           /**< Structured metric metadata */
        WorkflowMetricValueType     valueType;          /**< Type of metric value stored */
        std::atomic<std::uint64_t>  intValue = 0;       /**< Atomic integer metric value */
        std::atomic<double>         doubleValue = 0.0;  /**< Atomic double metric value */

        /**
         * @brief Constructs a registered atomic metric.
         * @param name Metric name.
         * @param unit Unit label for the metric value.
         * @param metadata Structured metric metadata.
         * @param type Metric value type.
         */
        AtomicMetric(QString name, QString unit, QVariantMap metadata, WorkflowMetricValueType type) :
    		name(std::move(name)),
            unit(std::move(unit)),
            metadata(std::move(metadata)),
            valueType(type)
        {
        }

        AtomicMetric(const AtomicMetric&) = delete;
        AtomicMetric& operator=(const AtomicMetric&) = delete;

        AtomicMetric(AtomicMetric&&) = delete;
        AtomicMetric& operator=(AtomicMetric&&) = delete;
    };

    mutable std::mutex                          _mutex;    /**< Protects registration and metric map access */
    std::unordered_map<QString, AtomicMetric>   _metrics;  /**< Registered metrics indexed by name */
};

}
