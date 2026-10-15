// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"
#include "WorkflowMetric.h"

#include <QString>
#include <QVariantMap>
#include <QVector>

#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace mv::workflow
{

/**
 * @brief Thread-safe accumulator for workflow execution metrics.
 *
 * Metrics are registered by name and then updated atomically during workflow
 * execution. snapshot() returns value objects suitable for reporting and
 * notifications.
 *
 * @maintainer Thomas Kroes (BioVault - Biomedical Visual Analytics Unit LUMC - TU Delft)
 */
class CORE_EXPORT WorkflowExecutionMetrics
{
public:

    /**
     * @brief Registers an integer metric.
     * @param name Metric name.
     * @param unit Metric unit label.
     * @param metadata Additional metric metadata.
     */
    void registerInteger(const QString& name, const QString& unit, QVariantMap metadata = {});

    /**
     * @brief Registers a floating-point metric.
     * @param name Metric name.
     * @param unit Metric unit label.
     * @param metadata Additional metric metadata.
     */
    void registerDouble(const QString& name, const QString& unit, QVariantMap metadata = {});

    /**
     * @brief Adds to an integer metric.
     * @param name Metric name.
     * @param amount Amount to add.
     */
    void addInteger(const QString& name, std::uint64_t amount);

    /**
     * @brief Adds to a floating-point metric.
     * @param name Metric name.
     * @param amount Amount to add.
     */
    void addDouble(const QString& name, double amount);

    /**
     * @brief Returns all registered metrics.
     * @return Snapshot of accumulated metric values.
     */
    [[nodiscard]] QVector<WorkflowMetric> snapshot() const;

private:

    /**
     * @brief Atomic storage for one workflow metric.
     */
    struct AtomicMetric
    {
        QString                     name;               /**< Metric name. */
        QString                     unit;               /**< Metric unit label. */
        QVariantMap                 metadata;           /**< Additional metric metadata. */
        WorkflowMetricValueType     valueType;          /**< Stored metric value type. */
        std::atomic<std::uint64_t>  intValue = 0;       /**< Integer metric value. */
        std::atomic<double>         doubleValue = 0.0;  /**< Floating-point metric value. */

        /**
         * @brief Constructs an atomic metric.
         * @param name Metric name.
         * @param unit Metric unit label.
         * @param metadata Additional metric metadata.
         * @param type Stored metric value type.
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

    mutable std::mutex                          _mutex;     /**< Protects the metric registry. */
    std::unordered_map<QString, AtomicMetric>   _metrics;   /**< Metrics indexed by name. */
};

}
