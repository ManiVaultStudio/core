// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DisplayComponentSpec.h"

namespace mv::util
{

DisplayComponentSpec::DisplayComponentSpec() :
    HardwareComponentSpec("Display Component Spec")
{
}

void DisplayComponentSpec::fromSystem()
{
    if (auto screen = QGuiApplication::primaryScreen())
        _resolution = { screen->size().width(), screen->size().height() };

    setInitialized();
}

void DisplayComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
    _resolution.first   = variantMap.value("horizontal", 0).toInt();
    _resolution.second  = variantMap.value("vertical", 0).toInt();
}

}
