// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

namespace mv::util
{

class OperationContext;

class CORE_EXPORT OperationContextScope
{
public:
    OperationContextScope(OperationContext* ctx);

    virtual ~OperationContextScope();

    static OperationContext* get();

private:
    OperationContext* _previous;

    static OperationContext*& current();
};

}
