// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <DockAreaTitleBar.h>

/**
 * Dock area title bar
 * 
 * Extends the ADS dock area title bar in order to add a settings hamburger menu tot the title bar.
 * 
 * @author Thomas Kroes
 */
class DockAreaTitleBar : public ads::CDockAreaTitleBar
{
public:

    /**
     * Construct with \p dockAreaWidget
     * @param dockAreaWidget Pointer to owning dock area widget
     */
    DockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget);
};