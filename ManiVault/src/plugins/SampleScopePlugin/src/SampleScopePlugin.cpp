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

        auto& widgetfader = _sampleScopeWidget.getNoSamplesOverlayWidget().getWidgetFader();

        if (canView)
            widgetfader.fadeOut();
        else
            widgetfader.fadeIn();
    };

    updateNoSamplesOverlayWidget();

    connect(&_sourcePluginPickerAction, &PluginPickerAction::pluginPicked, this, [this, updateNoSamplesOverlayWidget](Plugin* plugin) -> void {
        if (!plugin)
            return;

        if (_viewPluginSamplerAction) {
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, nullptr);
            disconnect(&_viewPluginSamplerAction->getSamplingModeAction(), &OptionAction::currentIndexChanged, this, nullptr);
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::canViewChanged, this, nullptr);
        }

        _viewPluginSamplerAction = dynamic_cast<ViewPluginSamplerAction*>(plugin->findChildByPath("Sampler"));

        if (_viewPluginSamplerAction) {
            const auto updateHtmlText = [this]() -> void {
                if (!_freezeViewAction.isChecked())
					_sampleScopeWidget.setHtmlText(_viewPluginSamplerAction->getViewString());
            };

            updateHtmlText();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, updateHtmlText);

            const auto updateFreezeActionReadOnly = [this]() -> void {
                _freezeViewAction.setEnabled(_viewPluginSamplerAction->getSamplingMode() == ViewPluginSamplerAction::SamplingMode::Selection);
			};

            updateFreezeActionReadOnly();

            connect(&_viewPluginSamplerAction->getSamplingModeAction(), &OptionAction::currentIndexChanged, this, updateFreezeActionReadOnly);

            updateNoSamplesOverlayWidget();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::canViewChanged, this, updateNoSamplesOverlayWidget);
        }
    });
}

void SampleScopePlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->addWidget(_horizontalGroupAction.createWidget(&getWidget()));
    layout->addWidget(&_sampleScopeWidget, 1);

    getWidget().setLayout(layout);
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

QIcon SampleScopePluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("microscope", color);
}

QUrl SampleScopePluginFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

ViewPlugin* SampleScopePluginFactory::produce()
{
    return new SampleScopePlugin(this);
}
