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
    _horizontalGroupAction(this, "Settings"),
    _tutorialPickerAction(this, "Pick tutorial"),
    _openInBrowserAction(this, "Open in browser")
{
    _horizontalGroupAction.setShowLabels(false);

    auto tutorialsModel = const_cast<mv::LearningCenterTutorialsModel*>(&mv::help().getTutorialsModel());

    _tutorialsFilterModel.setSourceModel(tutorialsModel);

    _tutorialPickerAction.setCustomModel(&_tutorialsFilterModel);
    _tutorialPickerAction.setPlaceHolderString("Pick a tutorial...");

    _openInBrowserAction.setIconByName("globe");
    _openInBrowserAction.setDefaultWidgetFlags(TriggerAction::Icon);

    auto& tagsFilterAction = _tutorialsFilterModel.getTagsFilterAction();

    tagsFilterAction.setDefaultWidgetFlags(OptionsAction::ComboBox | OptionsAction::Selection);
    tagsFilterAction.setPopupSizeHint(QSize(640, 0));

    _horizontalGroupAction.addAction(&_tutorialPickerAction, 1);
    _horizontalGroupAction.addAction(&tagsFilterAction);
    _horizontalGroupAction.addAction(&_openInBrowserAction);

    const auto currentTutorialChanged = [this, tutorialsModel]() -> void {
        const auto currentIndex = _tutorialPickerAction.getCurrentIndex();

        _openInBrowserAction.setEnabled(currentIndex >= 0);

        if (currentIndex < 0)
            return;

        const auto contentIndex         = _tutorialsFilterModel.index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Content));
        const auto urlIndex             = _tutorialsFilterModel.index(currentIndex, static_cast<int>(LearningCenterTutorialsModel::Column::Url));
        const auto sourceContentIndex   = _tutorialsFilterModel.mapToSource(contentIndex);
        const auto sourceUrlIndex       = _tutorialsFilterModel.mapToSource(urlIndex);
        const auto content              = tutorialsModel->data(sourceContentIndex, Qt::EditRole).toString();
        const auto url                  = tutorialsModel->data(sourceUrlIndex, Qt::EditRole).toString();

        _tutorialWidget.setHtmlText(content, url);
        _openInBrowserAction.setToolTip(QString("Go to: %1").arg(_tutorialWidget.getBaseUrl().toString()));
    };

    currentTutorialChanged();

    connect(&_tutorialPickerAction, &OptionAction::currentIndexChanged, this, currentTutorialChanged);

    connect(&_openInBrowserAction, &TriggerAction::triggered, this, [this]() -> void {
        QDesktopServices::openUrl(_tutorialWidget.getBaseUrl());
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
