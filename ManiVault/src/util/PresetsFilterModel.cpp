// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "PresetsFilterModel.h"
#include "PresetsModel.h"

namespace mv {

namespace util {

PresetsFilterModel::PresetsFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

bool PresetsFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return true;
}

}
}
