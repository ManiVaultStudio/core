// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DockComponentsFactory.h"

ads::CDockAreaTitleBar* DockComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) const
{
    return new DockAreaTitleBar(dockAreaWidget);
}
