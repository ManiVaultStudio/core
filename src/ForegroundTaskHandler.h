// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractTaskHandler.h"

namespace mv {

/**
 * Foreground task handler class
 *
 * Contains a bespoke tool button for the application that displays running foreground tasks in a popup.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ForegroundTaskHandler final : public AbstractTaskHandler
{
public:

    /**
    * Construct task handler with \p parent object
    * @param parent Pointer to parent object
    */
    ForegroundTaskHandler(QObject* parent);
};

}
