// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleScopePlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.SampleScopePlugin")

using namespace mv;
using namespace mv::gui;
using namespace mv::plugin;

SampleScopePlugin::SampleScopePlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _sampleScopeWidget(this, nullptr),
    _horizontalGroupAction(this, "Settings"),
    _sourcePluginPickerAction(this, "Source plugin"),
    _viewPluginSamplerAction(nullptr)
{
    _horizontalGroupAction.addAction(&_sourcePluginPickerAction);

    _sourcePluginPickerAction.setFilterPluginTypes({ Type::VIEW });
    _sourcePluginPickerAction.setFilterFunction([this](Plugin* plugin) -> bool {
        if (plugin == this)
            return false;

        if (auto viewPlugin = dynamic_cast<ViewPlugin*>(plugin))
            return viewPlugin->getSamplerAction().canView();

        return false;
    });

    connect(&_sourcePluginPickerAction, &PluginPickerAction::pluginPicked, this, [this](Plugin* plugin) -> void {
        if (!plugin)
            return;

        if (_viewPluginSamplerAction)
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, nullptr);

        _viewPluginSamplerAction = dynamic_cast<ViewPluginSamplerAction*>(plugin->findChildByPath("Sampler"));

        if (_viewPluginSamplerAction) {
            const auto updateHtmlText = [this]() -> void {
                _sampleScopeWidget.setHtmlText(_viewPluginSamplerAction->getViewString());
            };

            updateHtmlText();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::viewStringChanged, this, updateHtmlText);
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
