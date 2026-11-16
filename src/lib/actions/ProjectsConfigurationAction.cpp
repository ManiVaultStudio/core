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
    _defaultProjectResourceStagingPolicyAction(this, "Default resource staging policy", { "Read ranges on demand", "Stage on first read", "Stage immediately" }, "Read ranges on demand")
{
    _defaultProjectResourceStagingPolicyAction.setSerializationName("Default packed point-tile staging");
    addAction(&_projectsDsnsAction);
    addAction(&_defaultProjectResourceStagingPolicyAction);

    connect(Application::current(), &Application::coreInitialized, this, [this]() {
        const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel()).getDsnsAction().setStrings(_projectsDsnsAction.getStrings());
    });
}

void ProjectsConfigurationAction::fromVariantMap(const QVariantMap& variantMap)
{
    VerticalGroupAction::fromVariantMap(variantMap);

    _projectsDsnsAction.fromParentVariantMap(variantMap, true);
    auto stagingPolicyMap = variantMap.value(_defaultProjectResourceStagingPolicyAction.getSerializationName()).toMap();
    if (stagingPolicyMap.contains("Value"))
        stagingPolicyMap["Value"] = normalizeProjectResourceStagingPolicyName(stagingPolicyMap.value("Value").toString());
    if (stagingPolicyMap.contains("Options"))
    {
        auto stagingPolicyOptions = stagingPolicyMap.value("Options").toStringList();
        for (auto& option : stagingPolicyOptions)
            option = normalizeProjectResourceStagingPolicyName(option);
        stagingPolicyMap["Options"] = stagingPolicyOptions;
    }
    if (!stagingPolicyMap.isEmpty())
        _defaultProjectResourceStagingPolicyAction.fromVariantMap(stagingPolicyMap);
}

QVariantMap ProjectsConfigurationAction::toVariantMap() const
{
    auto variantMap = VerticalGroupAction::toVariantMap();

    _projectsDsnsAction.insertIntoVariantMap(variantMap);
    _defaultProjectResourceStagingPolicyAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
