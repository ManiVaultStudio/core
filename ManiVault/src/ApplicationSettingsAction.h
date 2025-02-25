// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/StringAction.h"
#include "actions/OptionAction.h"
#include "actions/HorizontalGroupAction.h"

namespace mv::gui
{

/**
 * Application global settings action class
 *
 * Action class which groups all global settings for the application
 *
 * @author Thomas Kroes
 * @author Thomas Höllt
 */
class CORE_EXPORT ApplicationSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ApplicationSettingsAction(QObject* parent);

private:
    StringAction            _applicationSessionIdAction;            /** String action for the application session identifier */
    HorizontalGroupAction   _lightDarkSystemColorSchemeAction;      /** Groups the light and dark system color scheme actions together */
};

}
