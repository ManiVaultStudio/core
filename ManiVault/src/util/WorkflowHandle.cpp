// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WorkflowHandle.h"

namespace mv::util
{

WorkflowHandle::WorkflowHandle()
{
}

WorkflowHandle::WorkflowHandle(QUuid id, QString name):
	_id(std::move(id)),
	_name(std::move(name))
{
}

bool WorkflowHandle::isValid() const
{
	return !_id.isNull();
}

QUuid WorkflowHandle::getId() const
{
	return _id;
}

QString WorkflowHandle::getName() const
{
	return _name;
}

}
