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

class CORE_EXPORT WorkflowExecutionMetrics
{
public:
    void registerInteger(const QString& name, const QString& unit, QVariantMap metadata = {});
    void registerDouble(const QString& name, const QString& unit, QVariantMap metadata = {});

    void addInteger(const QString& name, std::uint64_t amount);
    void addDouble(const QString& name, double amount);

    QVector<WorkflowMetric> snapshot() const;

private:
    struct AtomicMetric
    {
        QString                     name;
        QString                     unit;
        QVariantMap                 metadata;
        WorkflowMetricValueType     valueType;
        std::atomic<std::uint64_t>  intValue = 0;
        std::atomic<double>         doubleValue = 0.0;

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

    mutable std::mutex                          _mutex;
    std::unordered_map<QString, AtomicMetric>   _metrics;
};

}