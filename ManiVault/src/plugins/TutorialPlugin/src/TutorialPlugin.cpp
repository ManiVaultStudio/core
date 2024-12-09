// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TutorialPlugin.h"

#include <Application.h>
#include <CoreInterface.h>

Q_PLUGIN_METADATA(IID "studio.manivault.TutorialPlugin")

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

TutorialPlugin::TutorialPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _tutorialWidget(this, nullptr),
    _horizontalGroupAction(this, "Settings"),
    _tutorialPickerAction(this, "Pick tutorial")
{
    auto tutorialsModel = const_cast<mv::LearningCenterTutorialsModel*>(&mv::help().getTutorialsModel());

    _learningCenterTutorialsFilterModel.setSourceModel(tutorialsModel);

    _tutorialPickerAction.setCustomModel(tutorialsModel);
    _tutorialPickerAction.setPlaceHolderString("Pick a tutorial...");

    auto& tagsFilterAction = _learningCenterTutorialsFilterModel.getTagsFilterAction();

    tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup, true);
    tagsFilterAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup, true);

    _horizontalGroupAction.addAction(&_tutorialPickerAction);
    _horizontalGroupAction.addAction(&tagsFilterAction, OptionsAction::Tags | OptionsAction::Selection);

    connect(&_tutorialPickerAction, &OptionAction::currentIndexChanged, this, [this, tutorialsModel](const int& currentIndex) -> void {
        const auto contentIndex = tutorialsModel->index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Content));
        const auto urlIndex     = tutorialsModel->index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Url));
        const auto content      = tutorialsModel->data(contentIndex, Qt::EditRole).toString();
        const auto url          = tutorialsModel->data(urlIndex, Qt::EditRole).toString();

        _tutorialWidget.setHtmlText(content, url);
    });
}

void TutorialPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->addWidget(_horizontalGroupAction.createWidget(&getWidget()));
    layout->addWidget(&_tutorialWidget, 1);

    getWidget().setLayout(layout);
}

void TutorialPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    //_sourcePluginPickerAction.fromParentVariantMap(variantMap);
    //_freezeViewAction.fromParentVariantMap(variantMap);
}

QVariantMap TutorialPlugin::toVariantMap() const
{
    auto variantMap = ViewPlugin::toVariantMap();

    //_sourcePluginPickerAction.insertIntoVariantMap(variantMap);
    //_freezeViewAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

QIcon TutorialPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("chalkboard", color);
}

QUrl TutorialPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

ViewPlugin* TutorialPluginFactory::produce()
{
    return new TutorialPlugin(this);
}
