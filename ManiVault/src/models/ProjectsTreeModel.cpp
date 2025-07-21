// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectsTreeModel.h"

#include "CoreInterface.h"

#include "util/FileDownloader.h"
#include "util/JSON.h"

#include <nlohmann/json.hpp>

#include <QtConcurrent>

#ifdef _DEBUG
    //#define PROJECTS_TREE_MODEL_VERBOSE
#endif

#define PROJECTS_TREE_MODEL_VERBOSE

using namespace mv::util;
using namespace mv::gui;

using nlohmann::json;

namespace mv {

ProjectsTreeModel::ProjectsTreeModel(const PopulationMode& populationMode /*= Mode::Automatic*/, QObject* parent /*= nullptr*/) :
    AbstractProjectsModel(populationMode, parent)
{
}

void ProjectsTreeModel::populateFromDsns()
{
    if (mv::plugins().isInitializing())
        return;

    if (!mv::core()->isInitialized())
        return;

//    QStringList pluginDsns;
//
//    for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes())
//        pluginDsns << pluginFactory->getProjectsDsnsAction().getStrings();
//
//    pluginDsns.removeDuplicates();
//
//    auto nonPluginDsns = getDsnsAction().getStrings();
//
//    for (const auto& pluginDsn : pluginDsns)
//        nonPluginDsns.removeAll(pluginDsn);
//
//#ifdef PROJECTS_TREE_MODEL_VERBOSE
//    qDebug() << QString("Populating projects tree model from plugin DSNs (plugin DSNs:%1, non-plugin DSNs: %2)").arg(pluginDsns.join(", "), nonPluginDsns.join(", "));
//#endif
//
//    getDsnsAction().setStrings(pluginDsns + nonPluginDsns);
//    getDsnsAction().setLockedStrings(pluginDsns);
//
//#ifdef PROJECTS_TREE_MODEL_VERBOSE
//    qDebug() << "Populating projects tree model from DSNs";
//#endif

    
}



}
