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

bool CpuComponentSpec::meets(const HardwareComponentSpec& required) const
{
    return *this == dynamic_cast<const CpuComponentSpec&>(required) || *this > dynamic_cast<const CpuComponentSpec&>(required);
}

QString CpuComponentSpec::getFailureString(const HardwareComponentSpec& required) const
{
	if (meets(required))
		return {};

	const auto& cpuComponentSpec = dynamic_cast<const CpuComponentSpec&>(required);

	return "CPU does not meet requirements";
}

QStandardItem* CpuComponentSpec::getStandardItem() const
{
	auto item = HardwareComponentSpec::getStandardItem();

    item->setIcon(StyledIcon("microchip"));

    return item;
}

void CpuComponentSpec::fromSystem()
{
}

void CpuComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
	HardwareComponentSpec::fromVariantMap(variantMap);
}


}
