// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PluginFactoriesFilterModel.h"
#include "AbstractPluginFactoriesModel.h"

#include <QDebug>

#ifdef _DEBUG
    #define PLUGIN_FACTORIES_FILTER_MODEL_VERBOSE
#endif

namespace mv {

using namespace gui;

PluginFactoriesFilterModel::PluginFactoriesFilterModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

}