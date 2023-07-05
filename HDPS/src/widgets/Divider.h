// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QFrame>

class QWidget;

namespace hdps {

namespace gui {

/** Create horizontal divider line */
QWidget* createHorizontalDivider();

/** Create vertical divider line */
QWidget* createVerticalDivider();

}
}
