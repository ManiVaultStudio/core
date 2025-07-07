// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpec.h"

#include "DisplayComponentSpec.h"

namespace mv::util
{

HardwareSpec HardwareSpec::systemHardwareSpec;

HardwareSpec::HardwareSpec()
{
    _componentSpecs.push_back(std::make_shared<DisplayComponentSpec>());
}

void HardwareSpec::fromVariantMap(const QVariantMap& variantMap)
{
	Serializable::fromVariantMap(variantMap);

    for (auto& componentSpec : _componentSpecs)
        componentSpec->fromVariantMap(variantMap);
}

void HardwareSpec::fromSystem() const
{
    for (auto& componentSpec : _componentSpecs)
        componentSpec->fromSystem();
}

bool HardwareSpec::isInitialized() const
{
    auto initialized = false;

    for (auto& componentSpec : _componentSpecs)
        if (componentSpec->isInitialized())
            initialized = true;

    return initialized;
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
