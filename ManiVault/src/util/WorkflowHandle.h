// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QUuid>
#include <QString>

namespace mv::util
{

class CORE_EXPORT WorkflowHandle
{
public:
    WorkflowHandle();

    explicit WorkflowHandle(QUuid id, QString name = {});

    bool isValid() const;

    QUuid getId() const;

    QString getName() const;

private:
    QUuid _id;
    QString _name;
};

}