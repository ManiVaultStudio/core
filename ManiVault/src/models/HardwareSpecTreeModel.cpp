// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpecTreeModel.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    //#define HARDWARE_SPECS_TREE_MODEL_VERBOSE
#endif

using namespace mv::util;
using namespace mv::gui;

namespace mv {

HardwareSpecTreeModel::HardwareSpecTreeModel(QObject* parent /*= nullptr*/) :
    AbstractHardwareSpecModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));
}

void HardwareSpecTreeModel::setHardwareSpec(const util::HardwareSpec& hardwareSpec)
{
    setRowCount(0);

    for (const auto& hardwareComponentSpec : hardwareSpec.getHardwareComponentSpecs())
        if (hardwareComponentSpec->isInitialized())
    		appendRow(hardwareComponentSpec->getStandardItem());
}

}
