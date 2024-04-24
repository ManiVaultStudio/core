// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SortFilterProxyModel.h"

#include <QDebug>

#ifdef _DEBUG
    //#define FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;

namespace mv
{

SortFilterProxyModel::SortFilterProxyModel(QObject* parent /*= nullptr*/) :
    QSortFilterProxyModel(parent),
    _numberOfRowsAction(this, "Number of rows")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);

    _numberOfRowsAction.initialize(this);

    invalidate();
}

}
