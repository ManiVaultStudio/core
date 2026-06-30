// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultBase.h"

namespace mv::workflow
{

WorkflowResultBase::WorkflowResultBase(const QString& workflowName) :
    _workflowName(workflowName)
{
}

QString WorkflowResultBase::getWorkflowName() const
{
    return _workflowName;
}

WorkflowResultBase::Status WorkflowResultBase::getStatus() const
{
    return _status;
}

void WorkflowResultBase::setStatus(Status status)
{
    _status = status;
}

bool WorkflowResultBase::hasValue() const
{
    return _value.isValid();
}

const QVariant& WorkflowResultBase::getValue() const
{
    return _value;
}

void WorkflowResultBase::setValue(const QVariant& value)
{
    _value = value;
}

void WorkflowResultBase::setValue(QVariant&& value)
{
    _value = std::move(value);
}

}
