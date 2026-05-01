// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QDialog>
#include <QHash>
#include <QUuid>

namespace mv::util
{

class CORE_EXPORT WorkflowResultDialog : public QDialog
{
    Q_OBJECT

public:

    WorkflowResultDialog(const QUuid& workflowResultId, QWidget* parent = nullptr);

private:
};

} // namespace mv::util