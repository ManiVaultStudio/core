// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HardwareComponentSpec.h"

namespace mv::util
{

HardwareComponentSpec::HardwareComponentSpec(const QString& title) :
    _initialized(false),
    _title(title)
{
}

void HardwareComponentSpec::fromVariantMap(const QVariantMap& variantMap)
{
}

QStandardItem* HardwareComponentSpec::getStandardItem() const
{
    return new QStandardItem(_title);
}

QList<QStandardItem*> HardwareComponentSpec::getParameterRow(const QString& parameterName, const QString& systemValue /*= ""*/, const QString& requiredValue /*= ""*/, bool valid /*= true*/)
{
    return {
        new QStandardItem(valid ? StyledIcon("check-circle") : StyledIcon("exclamation-circle"), parameterName),
        new QStandardItem(systemValue),
        new QStandardItem(requiredValue),
    };
}

}
