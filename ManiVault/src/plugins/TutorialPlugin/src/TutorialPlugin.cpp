// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TutorialPlugin.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QDesktopServices>

Q_PLUGIN_METADATA(IID "studio.manivault.TutorialPlugin")

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

TutorialPlugin::TutorialPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _tutorialWidget(this, nullptr),
    _horizontalGroupAction(this, "Toolbar"),
    _tutorialPickerAction(this, "Pick tutorial"),
    _openInBrowserAction(this, "Open in browser"),
    _autoOpenProject(true)
{
    getLearningCenterAction().getToolbarVisibleAction().setChecked(false);

    _horizontalGroupAction.setShowLabels(false);

    auto tutorialsModel = const_cast<mv::LearningCenterTutorialsModel*>(&mv::help().getTutorialsModel());

    _tutorialsFilterModel.setSourceModel(tutorialsModel);
    _tutorialsFilterModel.getFilterGroupAction().setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    _tutorialsFilterModel.getTagsFilterAction().selectAll();
    _tutorialsFilterModel.getExcludeTagsFilterAction().initialize({ "Installation" }, { "Installation" });

    _tutorialPickerAction.setCustomModel(&_tutorialsFilterModel);
    _tutorialPickerAction.setPlaceHolderString("Pick a tutorial...");

    _openInBrowserAction.setIconByName("square-up-right");
    _openInBrowserAction.setDefaultWidgetFlags(TriggerAction::Icon);

    auto& tagsFilterAction = _tutorialsFilterModel.getTagsFilterAction();

    tagsFilterAction.setDefaultWidgetFlags(OptionsAction::Tags | OptionsAction::Selection);
    tagsFilterAction.setPopupSizeHint(QSize(500, 300));

    _horizontalGroupAction.addAction(&_tutorialPickerAction, 1);
    _horizontalGroupAction.addAction(&_tutorialsFilterModel.getFilterGroupAction());
    _horizontalGroupAction.addAction(&tagsFilterAction);
    _horizontalGroupAction.addAction(&_openInBrowserAction);
}

void TutorialPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->addWidget(_horizontalGroupAction.createWidget(&getWidget()));
    layout->addWidget(&_tutorialWidget, 1);

    getWidget().setLayout(layout);

    getLearningCenterAction().getToolbarVisibleAction().setChecked(false);

    connect(&_tutorialPickerAction, &OptionAction::currentIndexChanged, this, &TutorialPlugin::currentTutorialChanged);

    connect(&_openInBrowserAction, &TriggerAction::triggered, this, [this]() -> void {
        QDesktopServices::openUrl(_tutorialWidget.getBaseUrl());
    });

    // Not sure why this is needed, but without it the HTML is not displayed and the wen engine view is empty and the dock widget disappears
    _tutorialWidget.setHtmlText("", {});
}

const mv::util::LearningCenterTutorial* TutorialPlugin::getCurrentTutorial() const
{
    if (!_tutorialPickerAction.hasSelection())
        return {};

    const auto currentIndex = _tutorialPickerAction.getCurrentIndex();
    const auto sourceIndex  = _tutorialsFilterModel.mapToSource(_tutorialsFilterModel.index(currentIndex, 0));
    const auto tutorialItem = dynamic_cast<mv::LearningCenterTutorialsModel::Item*>(mv::help().getTutorialsModel().itemFromIndex(sourceIndex));

    if (!tutorialItem)
        return {};

    return tutorialItem->getTutorial();
}

void TutorialPlugin::currentTutorialChanged()
{
    const auto currentIndex = _tutorialPickerAction.getCurrentIndex();

    _openInBrowserAction.setEnabled(currentIndex >= 0);

    if (currentIndex < 0)
        return;

    auto tutorialsModel = const_cast<mv::LearningCenterTutorialsModel*>(&mv::help().getTutorialsModel());

    const auto contentIndex             = _tutorialsFilterModel.index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Content));
    const auto urlIndex                 = _tutorialsFilterModel.index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Url));
    const auto projectUrlIndex          = _tutorialsFilterModel.index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::ProjectUrl));
    const auto sourceContentIndex       = _tutorialsFilterModel.mapToSource(contentIndex);
    const auto sourceUrlIndex           = _tutorialsFilterModel.mapToSource(urlIndex);
    const auto sourceProjectUrlIndex    = _tutorialsFilterModel.mapToSource(projectUrlIndex);
    const auto content                  = tutorialsModel->data(sourceContentIndex, Qt::EditRole).toString();
    const auto url                      = tutorialsModel->data(sourceUrlIndex, Qt::EditRole).toString();
    const auto projectUrl               = tutorialsModel->data(sourceProjectUrlIndex, Qt::EditRole).toString();

    if (_autoOpenProject && !projectUrl.isEmpty()) {
        mv::projects().openProject(QUrl(projectUrl));
    }
    else {
        _tutorialWidget.setHtmlText(content, url);
        _openInBrowserAction.setToolTip(QString("Go to: %1").arg(_tutorialWidget.getBaseUrl().toString()));
    }
}

void TutorialPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    _autoOpenProject = false;
    {
	    _tutorialPickerAction.fromParentVariantMap(variantMap);
    }
    _autoOpenProject = true;

    getLearningCenterAction().getToolbarVisibleAction().setChecked(false);
}

QVariantMap TutorialPlugin::toVariantMap() const
{
    auto variantMap = ViewPlugin::toVariantMap();

    _tutorialPickerAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

TutorialPluginFactory::TutorialPluginFactory() :
	ViewPluginFactory(false, true)
{
    setIconByName("chalkboard");

    getPluginMetadata().setDescription("For showing tutorials");
    getPluginMetadata().setSummary("This view plugin shows tutorials and loads associated projects (if available).");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", { "Lead software architect" }, { "LUMC" } }
	});
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
    });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl TutorialPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

ViewPlugin* TutorialPluginFactory::produce()
{
    return new TutorialPlugin(this);
}
