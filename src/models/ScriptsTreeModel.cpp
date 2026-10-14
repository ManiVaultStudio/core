// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptsTreeModel.h"

using namespace mv;

#ifdef _DEBUG
    #define SCRIPTS_TREE_MODEL_VERBOSE
#endif

namespace mv {

ScriptsTreeModel::ScriptsTreeModel(QObject* parent /*= nullptr*/) :
    AbstractScriptsModel(parent)
{
}

}
