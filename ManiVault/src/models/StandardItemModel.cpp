// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StandardItemModel.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define STANDARD_ITEM_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;
using namespace gui;

StandardItemModel::StandardItemModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent),
    _numberOfRowsAction(this, "Number of rows")
{
    _numberOfRowsAction.initialize(this);
}

}
