// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PresetSerializationScope.h"

namespace mv::util {

thread_local bool PresetSerializationScope::_active = false;

PresetSerializationScope::Scope::Scope() :
    _previousActive(_active)
{
    _active = true;
}

PresetSerializationScope::Scope::~Scope()
{
    _active = _previousActive;
}

bool PresetSerializationScope::isActive()
{
    return _active;
}

}
