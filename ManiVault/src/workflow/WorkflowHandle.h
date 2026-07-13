// SPDX-License-Identifier: LGPL-3.0-or-later
// A corresponding LICENSE file is located in the root directory of this source tree
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)

#pragma once

#include "ManiVaultGlobals.h"

#include <QUuid>
#include <QString>

namespace mv::workflow
{

/**
 * @brief Lightweight identifier for workflow entities that can produce outputs.
 */
class CORE_EXPORT WorkflowHandle
{
public:

    /** Constructs an invalid workflow handle. */
    WorkflowHandle();

    /** Constructs a workflow handle. */
    explicit WorkflowHandle(QUuid id, QString name = {});

    /** @return True when the handle contains a non-null identifier. */
    bool isValid() const;

    /** @return Workflow entity identifier. */
    QUuid getId() const;

    /** @return Human-readable workflow entity name. */
    QString getName() const;

private:

    QUuid _id;      /**< Workflow entity identifier */
    QString _name;  /**< Human-readable workflow entity name */
};

}
