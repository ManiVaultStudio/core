// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractHardwareSpecModel.h"

#ifdef _DEBUG
    //#define ABSTRACT_HARDWARE_SPEC_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

AbstractHardwareSpecModel::AbstractHardwareSpecModel(QObject* parent /*= nullptr*/) :
    StandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

}
