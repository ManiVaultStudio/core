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
 * @brief Thread-safe accumulator for workflow execution metrics.
 */
class CORE_EXPORT WorkflowExecutionMetrics
{
public:

    /** Registers an integer metric. */
    void registerInteger(const QString& name, const QString& unit, QVariantMap metadata = {});

    /** Registers a floating-point metric. */
    void registerDouble(const QString& name, const QString& unit, QVariantMap metadata = {});

    /** Adds to an integer metric. */
    void addInteger(const QString& name, std::uint64_t amount);

    /** Adds to a floating-point metric. */
    void addDouble(const QString& name, double amount);

    /** @return Snapshot of all registered metrics. */
    QVector<WorkflowMetric> snapshot() const;

private:

    /** Atomic storage for one workflow metric. */
    struct AtomicMetric
    {
        QString                     name;               /**< Metric name */
        QString                     unit;               /**< Metric unit label */
        QVariantMap                 metadata;           /**< Additional metric metadata */
        WorkflowMetricValueType     valueType;          /**< Stored metric value type */
        std::atomic<std::uint64_t>  intValue = 0;       /**< Integer metric value */
        std::atomic<double>         doubleValue = 0.0;  /**< Floating-point metric value */

        /** Constructs an atomic metric. */
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

    mutable std::mutex                          _mutex;     /**< Protects the metric registry */
    std::unordered_map<QString, AtomicMetric>   _metrics;   /**< Metrics indexed by name */
};

}
