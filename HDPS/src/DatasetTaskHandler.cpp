// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DatasetTaskHandler.h"
#include "DatasetTask.h"

#include "CoreInterface.h"

namespace hdps {

using namespace gui;

DatasetTaskHandler::DatasetTaskHandler(QObject* parent /*= nullptr*/) :
    AbstractTaskHandler(parent)
{
}

void DatasetTaskHandler::init()
{
}

}
