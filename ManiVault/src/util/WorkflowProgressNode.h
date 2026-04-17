// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QDateTime>

namespace mv::util
{

class WorkflowProgressNode
{
public:
    using Ptr = std::shared_ptr<WorkflowProgressNode>;

    explicit WorkflowProgressNode(double weight = 1.0, WorkflowProgressNode* parent = nullptr)
        : _weight(weight)
        , _parent(parent)
    {
    }

    Ptr createChild(double weight)
    {
        QMutexLocker lock(&_mutex);

        auto child = std::make_shared<WorkflowProgressNode>(weight, this);
        _children.push_back(child);
        return child;
    }

    void setProgress(double value)
    {
        value = std::clamp(value, 0.0, 1.0);

        QMutexLocker lock(&_mutex);
        _selfProgress = value;
    }

    double getProgress() const
    {
        QMutexLocker lock(&_mutex);

        if (_children.isEmpty())
            return _selfProgress;

        double weightSum = 0.0;
        double weightedProgress = 0.0;

        const auto children = _children;
        lock.unlock();

        for (const auto& child : children) {
            const double weight = child->getWeight();
            weightSum += weight;
            weightedProgress += child->getProgress() * weight;
        }

        if (weightSum <= 0.0)
            return 0.0;

        return weightedProgress / weightSum;
    }

    double getWeight() const
    {
        QMutexLocker lock(&_mutex);
        return _weight;
    }

    WorkflowProgressNode* getParent() const
    {
        return _parent;
    }

private:
    double _weight = 1.0;
    double _selfProgress = 0.0;
    WorkflowProgressNode* _parent = nullptr;

    mutable QMutex _mutex;
    QVector<Ptr> _children;
};

} // namespace mv::util