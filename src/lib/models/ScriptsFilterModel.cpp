// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ScriptsFilterModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define SCRIPTS_FILTER_MODEL_VERBOSE
#endif

namespace mv {

ScriptsFilterModel::ScriptsFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

}