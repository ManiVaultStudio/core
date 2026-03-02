// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StandardItemModel.h"

#ifdef _DEBUG
    #define STANDARD_ITEM_MODEL_VERBOSE
#endif

namespace mv
{

using namespace util;
using namespace gui;

StandardItemModel::StandardItemModel(QObject* parent /*= nullptr*/, const QString& title /*= ""*/, const PopulationMode& populationMode /*= PopulationMode::Automatic*/) :
    QStandardItemModel(parent),
    Serializable(title),
    _populationMode(populationMode),
    _numberOfRowsAction(new NumberOfRowsAction(this, "Number of rows"))
{
    _numberOfRowsAction->initialize(this);
}

StandardItemModel::PopulationMode StandardItemModel::getPopulationMode() const
{
    return _populationMode;
}

void StandardItemModel::setPopulationMode(const PopulationMode& populationMode)
{
    if (_populationMode == populationMode)
        return;

    _populationMode = populationMode;

    emit populationModeChanged(_populationMode);
}

void StandardItemModel::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);
}

QVariantMap StandardItemModel::toVariantMap() const
{
    return Serializable::toVariantMap();
}
}
