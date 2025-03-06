// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Preset.h"

namespace mv::util {

Preset::Preset(const QString& name, const QString& description, const PresetType& presetType /*= PresetType::Normal*/) :
    _name(name),
    _description(description),
    _presetType(presetType),
    _isDivergent(false)
{
}

QString Preset::getName() const
{
    return _name;
}

QString Preset::getDescription() const
{
    return _description;
}

QIcon Preset::getIcon() const
{
    switch (_presetType)
    {
        case PresetType::Normal:
            return StyledIcon("sliders");

        case PresetType::Global:
            return StyledIcon("globe-europe");

        case PresetType::FactoryDefault:
            return StyledIcon("industry");
    }

    return {};
}

bool Preset::isDivergent() const
{
    return _isDivergent;
}

void Preset::isDivergent(bool isDivergent)
{
    _isDivergent = isDivergent;
}

}
