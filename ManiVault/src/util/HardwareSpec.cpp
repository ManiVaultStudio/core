// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpec.h"
#include "HardwareSpec.h"

#include "DisplayComponentSpec.h"

namespace mv::util
{

HardwareSpec HardwareSpec::systemHardwareSpec = HardwareSpec(HardwareSpec::Type::System);

HardwareSpec::HardwareSpec(const Type& type) :
    _type(type)
{
    _hardwareComponentSpecs.push_back(std::make_shared<DisplayComponentSpec>());
    _hardwareComponentSpecs.push_back(std::make_shared<CpuComponentSpec>());
    _hardwareComponentSpecs.push_back(std::make_shared<RamComponentSpec>());
}

void HardwareSpec::fromVariantMap(const QVariantMap& variantMap)
{
    for (auto& componentSpec : _hardwareComponentSpecs)
        componentSpec->fromVariantMap(variantMap);
}

void HardwareSpec::fromSystem() const
{
    for (auto& componentSpec : _hardwareComponentSpecs)
        componentSpec->fromSystem();
}

bool HardwareSpec::isInitialized() const
{
    auto initialized = false;

    for (auto& componentSpec : _hardwareComponentSpecs)
        if (componentSpec->isInitialized())
            initialized = true;

    return initialized;
}

HardwareComponentSpecs HardwareSpec::getHardwareComponentSpecs(bool initializedOnly /*= true*/) const
{
    if (!initializedOnly)
        return _hardwareComponentSpecs;

	HardwareComponentSpecs initializedHardwareComponentSpecs;

	for (const auto& candidateHardwareComponentSpec : _hardwareComponentSpecs)
        if (candidateHardwareComponentSpec->isInitialized())
            initializedHardwareComponentSpecs.push_back(candidateHardwareComponentSpec);

    return initializedHardwareComponentSpecs;
}

bool HardwareSpec::meets(const HardwareSpec& required) const
{
    return *this >= required;
}

QString HardwareSpec::getFailureString(const HardwareSpec& required) const
{
    if (meets(required))
        return "<p>System has the recommended hardware specification.</p>";

    QString status("<p>System does not have the recommended hardware specification:</p>");

    for (std::size_t i = 0; i < _hardwareComponentSpecs.size() && i < required._hardwareComponentSpecs.size(); ++i) {
        const auto lhs = _hardwareComponentSpecs[i];
        const auto rhs = required._hardwareComponentSpecs[i];

        if (lhs->isInitialized() && rhs->isInitialized() && !lhs->meets(*rhs))
        	status.append(QString("<p>%1</p>").arg(lhs->getFailureString(*rhs)));
    }

	return status;
}

const HardwareSpec& HardwareSpec::getSystemHardwareSpec()
{
    return systemHardwareSpec;
}

void HardwareSpec::updateSystemHardwareSpecs()
{
    systemHardwareSpec.fromSystem();
}

HardwareSpec::SystemCompatibilityInfo HardwareSpec::getSystemCompatibility(const HardwareSpec& minimumHardwareSpec, const HardwareSpec& recommendedHardwareSpec)
{
	if (!systemHardwareSpec.isInitialized() || !minimumHardwareSpec.isInitialized() || !recommendedHardwareSpec.isInitialized())
        return { SystemCompatibility::Unknown, QStringList() };

    if (systemHardwareSpec.meets(minimumHardwareSpec) && systemHardwareSpec.meets(recommendedHardwareSpec))
        return { SystemCompatibility::Compatible, QStringList() };

    if (!systemHardwareSpec.meets(minimumHardwareSpec)) {
        QStringList systemIncompatibilityReasons;

        return { SystemCompatibility::Incompatible, systemIncompatibilityReasons };
    }

    if (systemHardwareSpec.meets(minimumHardwareSpec) && !systemHardwareSpec.meets(recommendedHardwareSpec)) {
        QStringList systemIncompatibilityReasons;

        return { SystemCompatibility::Minimum, systemIncompatibilityReasons };
    }

    return { SystemCompatibility::Unknown, QStringList() };
}

}
