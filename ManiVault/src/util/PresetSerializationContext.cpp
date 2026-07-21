// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PresetSerializationContext.h"

namespace mv::util {

thread_local bool PresetSerializationContext::_active = false;

PresetSerializationContext::Scope::Scope() :
    _previousActive(_active)
{
    _active = true;
}

PresetSerializationContext::Scope::~Scope()
{
    _active = _previousActive;
}

bool PresetSerializationContext::isActive()
{
    return _active;
}

}
