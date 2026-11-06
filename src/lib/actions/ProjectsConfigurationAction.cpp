// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsConfigurationAction.h"
#include "Application.h"

#include "CoreInterface.h"

#include "models/ProjectsTreeModel.h"

using namespace mv::util;

namespace mv::gui {

ProjectsConfigurationAction::ProjectsConfigurationAction(QObject* parent, const QString& title) :
    VerticalGroupAction(parent, title),
    _projectsDsnsAction(this, "Projects"),
    _defaultPackedTileStagingPolicyAction(this, "Default packed point-tile staging", { "Archive-backed", "Stage on demand", "Stage before load" }, "Archive-backed")
{
    addAction(&_projectsDsnsAction);
    addAction(&_defaultPackedTileStagingPolicyAction);

    connect(Application::current(), &Application::coreInitialized, this, [this]() {
        const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel()).getDsnsAction().setStrings(_projectsDsnsAction.getStrings());
    });
}

void ProjectsConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _projectsDsnsAction.fromParentVariantMap(variantMap, true);
    _defaultPackedTileStagingPolicyAction.fromParentVariantMap(variantMap, true);
}

QVariantMap ProjectsConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _projectsDsnsAction.insertIntoVariantMap(variantMap);
    _defaultPackedTileStagingPolicyAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
