// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

class QWidget;

namespace mv {

namespace gui {

/** Create horizontal divider line */
CORE_EXPORT QWidget* createHorizontalDivider();

/** Create vertical divider line */
CORE_EXPORT QWidget* createVerticalDivider();

}
}
