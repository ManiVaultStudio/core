// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpec.h"

namespace mv::util
{

HardwareSpec HardwareSpec::systemHardwareSpec;

void HardwareSpec::fromVariantMap(const QVariantMap& variantMap)
{
	Serializable::fromVariantMap(variantMap);
}

void HardwareSpec::fromSystem() const
{
    for (auto& componentSpec : _componentSpecs)
        componentSpec->fromSystem();
}

const HardwareSpec& HardwareSpec::getSystemHardwareSpec()
{
    return systemHardwareSpec;
}

void HardwareSpec::updateSystemHardwareSpecs()
{
    systemHardwareSpec.fromSystem();
}

}
