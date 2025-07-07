// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CpuComponentSpec.h"

namespace mv::util
{

CpuComponentSpec::CpuComponentSpec() :
    HardwareComponentSpec("CPU Component Spec")
{
}

void CpuComponentSpec::fromSystem()
{
}

void CpuComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
	HardwareComponentSpec::fromVariantMap(variantMap);
}

}
