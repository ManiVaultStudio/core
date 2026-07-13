// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowResultBase.h"

#include "util/StyledIcon.h"

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

	switch (_status) {
		case Status::Undefined:
			return mv::util::StyledIcon("question");

		case Status::Success:
			return mv::util::StyledIcon("square-check");

		case Status::Failed:
			return mv::util::StyledIcon("triangle-exclamation");

		case Status::Canceled:
			return mv::util::StyledIcon("square-xmark");

		default:
			return mv::util::StyledIcon("question");
	}
}

void WorkflowResultBase::setStatus(Status status)
{
    _status = status;
}

void WorkflowResultBase::setOptions(const WorkflowOptions& options)
{
	_options = options;
}

const WorkflowOptions& WorkflowResultBase::getOptions() const
{
	return _options;
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
