// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareSpecFilterModel.h"
#include "AbstractHardwareSpecModel.h"

#include "util/Version.h"

#include "Application.h"

#include <QDebug>

#ifdef _DEBUG
    #define HARDWARE_SPEC_FILTER_MODEL_VERBOSE
#endif

using namespace mv::gui;
using namespace mv::util;

namespace mv {

HardwareSpecFilterModel::HardwareSpecFilterModel(QObject* parent /*= nullptr*/) :
    SortFilterProxyModel(parent, "HardwareSpecFilterModel")
{
    setDynamicSortFilter(true);
    setRecursiveFilteringEnabled(true);
    setRowTypeName("Hardware component spec");
}

bool HardwareSpecFilterModel::filterAcceptsRow(int row, const QModelIndex& parent) const
{
    return SortFilterProxyModel::filterAcceptsRow(row, parent);
}

}
