// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPageVideosModel.h"

#ifdef _DEBUG
    //#define LEARNING_PAGE_VIDEOS_MODEL_VERBOSE
#endif

LearningPageVideosModel::LearningPageVideosModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
}
