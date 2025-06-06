// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptsListModel.h"

#ifdef _DEBUG
    #define SCRIPTS_LIST_MODEL_VERBOSE
#endif

namespace mv {

ScriptsListModel::ScriptsListModel(QObject* parent /*= nullptr*/) :
    AbstractScriptsModel(parent)
{
}

}
