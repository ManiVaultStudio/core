// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ColorMap2DAction.h"

using namespace mv::util;

namespace mv::gui {

ColorMap2DAction::ColorMap2DAction(QObject* parent, const QString& title, const QString& colorMap /*= "example_a"*/) :
    ColorMapAction(parent, title, ColorMap::Type::TwoDimensional, colorMap)
{
}

}
