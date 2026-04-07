// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OperationContextScope.h"

namespace mv::util
{

OperationContextScope::OperationContextScope(OperationContext* ctx)
{
	_previous = current();
	current() = ctx;
}

OperationContextScope::~OperationContextScope()
{
	current() = _previous;
}

OperationContext* OperationContextScope::get()
{
	return current();
}

OperationContext*& OperationContextScope::current()
{
	thread_local OperationContext* ctx = nullptr;
	return ctx;
}

}
