// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsConfigurationAction.h"

using namespace mv::util;

namespace mv::gui {

ProjectsConfigurationAction::ProjectsConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _projectsDsnsAction(this, "Projects")
{
    addAction(&_projectsDsnsAction);
}

void ProjectsConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _projectsDsnsAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ProjectsConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _projectsDsnsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
