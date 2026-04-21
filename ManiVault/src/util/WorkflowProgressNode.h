// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QString>
#include <QDateTime>
#include <QMutex>
#include <QVector>
#include <QDebug>

namespace mv::util
{

class WorkflowProgressNode
{
public:
    using Ptr = std::shared_ptr<WorkflowProgressNode>;

    explicit WorkflowProgressNode(double weight = 1.0);

    Ptr createChild(double weight);

    bool hasChildren() const;

    void setProgress(double value);

    double getProgress() const;

    double getWeight() const;

private:
    double _weight = 1.0;
    double _selfProgress = 0.0;

    mutable QMutex _mutex;
    QVector<Ptr> _children;
};

} // namespace mv::util