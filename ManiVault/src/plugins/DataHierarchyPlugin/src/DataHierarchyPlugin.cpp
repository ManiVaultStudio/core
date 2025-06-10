// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>
#include <memory>
#include <PointData/PointData.h>

#include <util/Script.h>
#include <util/PythonScript.h>

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")using namespace mv;
using namespace mv::gui;
using namespace mv::util;

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataHierarchyWidget(nullptr)
{
    auto& shortcuts = getShortcuts();

    shortcuts.add({ QKeySequence(Qt::CTRL | Qt::Key_Minus), "Expand/collapse", "Collapse all" });
    shortcuts.add({ QKeySequence(Qt::CTRL | Qt::Key_Plus), "Expand/collapse", "Expand all" });

    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "DataHierarchyPlugin" }));
}

void DataHierarchyPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataHierarchyWidget);

    getWidget().setLayout(layout);
}

DataHierarchyPluginFactory::DataHierarchyPluginFactory() :
    ViewPluginFactory(true)
{
    setIconByName("sitemap");

    getPluginMetadata().setDescription("Hierarchical overview of all loaded data");
    getPluginMetadata().setSummary("This plugin is for interacting with the ManiVault Studio dataset hierarchy");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", { "Lead software architect" }, { "LUMC" } },
        { "A. Vieth", { "Plugin developer", "Maintainer" }, { "LUMC", "TU Delft" } },
        { "J. Thijssen", { "Software architect" }, { "LUMC", "TU Delft" } }
    });
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
    });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

void DataHierarchyPluginFactory::initialize()
{
	ViewPluginFactory::initialize();
}

QUrl DataHierarchyPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/DataHierarchyPlugin/README.md");
#else
    return { "https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/DataHierarchyPlugin/README.md" };
#endif
}

QUrl DataHierarchyPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core/tree/master/ManiVault/src/plugins/DataHierarchyPlugin" };
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}

ScriptTriggerActions DataHierarchyPluginFactory::getScriptTriggerActions(const mv::Datasets& datasets) const
{
    ScriptTriggerActions scriptTriggerActions;

    if (areAllDatasetsOfTheSameType(datasets, PointType) && datasets.count() >= 2) {
        auto scriptTriggerAction = new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Stitch", Script::Type::Transformation, "", datasets), "Transformation/Stitch");

        scriptTriggerActions << scriptTriggerAction;
    }

    if (datasets.count() == 1 && datasets.first()->getDataType() == PointType) {
        scriptTriggerActions << new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Export BIN", Script::Type::Writer, "", datasets), "Export/BIN");
        scriptTriggerActions << new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Export PNG", Script::Type::Writer, "", datasets), "Export/JSON");
        scriptTriggerActions << new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Average", Script::Type::Analysis, "", datasets), "Analysis/Average");
        scriptTriggerActions << new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Min", Script::Type::Analysis, "", datasets), "Analysis/Min");
        scriptTriggerActions << new ScriptTriggerAction(nullptr, std::make_shared<PythonScript>("Max", Script::Type::Analysis, "", datasets), "Analysis/Max");
    }

    if (datasets.count() == 1 && datasets.first()->getDataType() == PointType) {
        
    }

	return scriptTriggerActions;
}
