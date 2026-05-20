// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "OperationContextScope.h"

namespace mv::util
{

OperationContextScope::OperationContextScope(SharedOperationContext context): _previous(currentShared())
{
	currentShared() = std::move(context);
}

OperationContextScope::~OperationContextScope()
{
	currentShared() = _previous;
}

OperationContext* OperationContextScope::get()
{
	return currentShared().get();
}

SharedOperationContext OperationContextScope::getShared()
{
	return currentShared();
}

SharedOperationContext& OperationContextScope::currentShared()
{
	thread_local SharedOperationContext context;
	return context;
}

}
