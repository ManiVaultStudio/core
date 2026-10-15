// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HeadsUpDisplayItem.h"
#include "Serializable.h"

namespace mv::util
{

HeadsUpDisplayItem::HeadsUpDisplayItem(const QString& title, const QString& value, const QString& description, const HeadsUpDisplayItemSharedPtr& parent /*= nullptr*/) :
    _id(Serializable::createId()),
    _title(title),
    _value(value),
    _description(description),
    _parent(parent)
{
}

void HeadsUpDisplayItem::setIndex(const QPersistentModelIndex& index)
{
    _index = index;
}

}
