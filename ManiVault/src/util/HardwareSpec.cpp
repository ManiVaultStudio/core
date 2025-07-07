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

QString HardwareSpec::getStatusString(const HardwareSpec& other) const
{
    if (*this > other)
        return "<p>System has the recommended hardware specification.</p>";

    QString status("<p>System does not have the recommended hardware specification:</p>");

    for (std::size_t i = 0; i < _componentSpecs.size() && i < other._componentSpecs.size(); ++i) {
        const auto& lhs = _componentSpecs[i];
        const auto& rhs = other._componentSpecs[i];

        if (lhs->isInitialized() && rhs->isInitialized())
            if (const auto statusString = lhs->getStatusString(rhs)))
                status.append(QString("<p>%1</p>").arg(statusString));
    }

	return status;
}

HardwareComponentSpecs HardwareSpec::getFailingHardwareComponentSpecs(const HardwareSpec& other) const
{
    HardwareComponentSpecs failingHardwareComponentSpecs;

    for (std::size_t i = 0; i < _componentSpecs.size() && i < other._componentSpecs.size(); ++i) {
        const auto& lhs = _componentSpecs[i];
        const auto& rhs = other._componentSpecs[i];

        if (lhs->isInitialized() && rhs->isInitialized())
            failingHardwareComponentSpecs.push_back(lhs);
    }

    return failingHardwareComponentSpecs;
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
