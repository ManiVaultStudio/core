// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopePlugin.h"

#include <Application.h>
#include <actions/ViewPluginSamplerAction.h>

Q_PLUGIN_METADATA(IID "studio.manivault.SampleScopePlugin")

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

SampleScopePlugin::SampleScopePlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _sampleScopeWidget(this, nullptr),
    _horizontalGroupAction(this, "Settings"),
    _sourcePluginPickerAction(this, "Source plugin"),
    _freezeViewAction(this, "Freeze view"),
    _viewPluginSamplerAction(nullptr)
{
    _sourcePluginPickerAction.setToolTip("The view plugin to display the samples for");
    _freezeViewAction.setToolTip("Freeze the current view");

    _horizontalGroupAction.addAction(&_sourcePluginPickerAction);
    _horizontalGroupAction.addAction(&_freezeViewAction);

    _sourcePluginPickerAction.setStretch(1);
    _sourcePluginPickerAction.setFilterPluginTypes({ Type::VIEW });
    _sourcePluginPickerAction.setFilterFunction([this](Plugin* plugin) -> bool {
        if (plugin == this)
            return false;

		return dynamic_cast<ViewPlugin*>(plugin);
    });

    const auto updateNoSamplesOverlayWidget = [this]() -> void {
        
        const auto canView = _viewPluginSamplerAction && _viewPluginSamplerAction->canView();

        auto& widgetFader = _sampleScopeWidget.getNoSamplesOverlayWidget().getWidgetFader();

        if (canView)
            widgetFader.fadeOut();
        else
            widgetFader.fadeIn();

        widgetFader.fadeIn();
    };

    updateNoSamplesOverlayWidget();

    connect(&_sourcePluginPickerAction, &PluginPickerAction::pluginPicked, this, [this, updateNoSamplesOverlayWidget](Plugin* plugin) -> void {
        if (!plugin)
            return;

        if (_viewPluginSamplerAction) {
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, nullptr);
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewWidgetChanged, this, nullptr);
            disconnect(&_viewPluginSamplerAction->getSamplingModeAction(), &OptionAction::currentIndexChanged, this, nullptr);
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::canViewChanged, this, nullptr);
        }

        _viewPluginSamplerAction = dynamic_cast<ViewPluginSamplerAction*>(plugin->findChildByPath("Sampler"));

        if (_viewPluginSamplerAction) {
            const auto updateViewHtml = [this]() -> void {
                _freezeViewAction.setVisible(true);

                if (!_freezeViewAction.isChecked())
					_sampleScopeWidget.setViewHtml(_viewPluginSamplerAction->getViewString());
            };

            if (_viewPluginSamplerAction->getViewGeneratorType() == ViewPluginSamplerAction::ViewGeneratorType::HTML)
				updateViewHtml();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, updateViewHtml);

            const auto updateViewWidget = [this]() -> void {
                _freezeViewAction.setVisible(false);

				_sampleScopeWidget.setViewWidget(_viewPluginSamplerAction->getViewWidget());
            };

            if (_viewPluginSamplerAction->getViewGeneratorType() == ViewPluginSamplerAction::ViewGeneratorType::Widget)
				updateViewWidget();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewWidgetChanged, this, updateViewWidget);

            const auto updateFreezeActionReadOnly = [this]() -> void {
                _freezeViewAction.setEnabled(_viewPluginSamplerAction->getSamplingMode() == ViewPluginSamplerAction::SamplingMode::Selection);
			};

            updateFreezeActionReadOnly();

            connect(&_viewPluginSamplerAction->getSamplingModeAction(), &OptionAction::currentIndexChanged, this, updateFreezeActionReadOnly);

            updateNoSamplesOverlayWidget();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::canViewChanged, this, updateNoSamplesOverlayWidget);
        }
    });

    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "SampleScopePlugin" }));

    _sampleScopeWidget.initialize();
}

void SampleScopePlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->addWidget(_horizontalGroupAction.createWidget(&getWidget()));
    layout->addWidget(&_sampleScopeWidget, 1);

    getWidget().setLayout(layout);
}

ViewPluginSamplerAction* SampleScopePlugin::getViewPluginSamplerAction() const
{
	return _viewPluginSamplerAction;
}

void SampleScopePlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);

    _sourcePluginPickerAction.fromParentVariantMap(variantMap);
    _freezeViewAction.fromParentVariantMap(variantMap);
}

QVariantMap SampleScopePlugin::toVariantMap() const
{
    auto variantMap = ViewPlugin::toVariantMap();

    _sourcePluginPickerAction.insertIntoVariantMap(variantMap);
    _freezeViewAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

SampleScopePluginFactory::SampleScopePluginFactory()
{
    setIconByName("microscope");

    getPluginMetadata().setDescription("For displaying sample information from another view plugin");
    getPluginMetadata().setSummary("This view plugin displays sample information obtained from another view plugin.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", { "Lead software architect" }, { "LUMC" } },
	});
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
	});
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl SampleScopePluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

ViewPlugin* SampleScopePluginFactory::produce()
{
    return new SampleScopePlugin(this);
}
