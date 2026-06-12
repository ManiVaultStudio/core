// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QObject>
#include <QHash>
#include <QUuid>

namespace mv::workflow
{

class CORE_EXPORT WorkflowResultRegistry : public QObject
{
    Q_OBJECT

public:
    static WorkflowResultRegistry& instance();

    QUuid add(SharedWorkflowResult result);

    SharedWorkflowResult get(const QUuid& id) const;

    void remove(const QUuid& id);

private:
    QHash<QUuid, SharedWorkflowResult> _results;
};

} // namespace mv::util