// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowExecutionMetrics.h"

namespace mv::workflow
{

void WorkflowExecutionMetrics::registerInteger(const QString& name, const QString& unit, QVariantMap metadata)
{
    std::scoped_lock lock(_mutex);

    if (_metrics.contains(name))
        return;

    _metrics.try_emplace(name, name, unit, std::move(metadata),WorkflowMetricValueType::Integer);
}

void WorkflowExecutionMetrics::registerDouble(const QString& name, const QString& unit, QVariantMap metadata)
{
    std::scoped_lock lock(_mutex);

    if (_metrics.contains(name))
        return;

    _metrics.try_emplace(name, name, unit, std::move(metadata), WorkflowMetricValueType::FloatingPoint);
}

void WorkflowExecutionMetrics::addInteger(const QString& name, std::uint64_t amount)
{
    std::scoped_lock lock(_mutex);

    auto it = _metrics.find(name);

    if (it == _metrics.end())
        return;

    const auto before = it->second.intValue.load(std::memory_order_relaxed);

	it->second.intValue.fetch_add(amount, std::memory_order_relaxed);

	const auto after = it->second.intValue.load(std::memory_order_relaxed);
}

void WorkflowExecutionMetrics::addDouble(const QString& name, double amount)
{
    std::scoped_lock lock(_mutex);

    auto it = _metrics.find(name);

    if (it == _metrics.end())
        return;

    if (it->second.valueType != WorkflowMetricValueType::FloatingPoint)
        return;

    double current = it->second.doubleValue.load(std::memory_order_relaxed);

    while (!it->second.doubleValue.compare_exchange_weak(current, current + amount, std::memory_order_relaxed)) {
        // retry
    }
}

QVector<WorkflowMetric> WorkflowExecutionMetrics::snapshot() const
{
    std::scoped_lock lock(_mutex);

    QVector<WorkflowMetric> result;

    result.reserve(static_cast<qsizetype>(_metrics.size()));

    for (const auto& [_, metric] : _metrics) {

        QVariant value;

        if (metric.valueType == WorkflowMetricValueType::Integer)
            value = static_cast<qulonglong>(metric.intValue.load(std::memory_order_relaxed));
        else
            value = metric.doubleValue.load(std::memory_order_relaxed);

        result.append(WorkflowMetric{ metric.name, metric.unit, value, metric.metadata });
    }

    return result;
}

}
