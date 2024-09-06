// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleContextPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.SampleContextPlugin")

using namespace mv;
using namespace mv::gui;

SampleContextPlugin::SampleContextPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _sampleContextWidget(this, nullptr),
    _horizontalGroupAction(this, "Settings"),
    _sourcePluginPickerAction(this, "Source plugin"),
    _viewPluginSamplerAction(nullptr)
{
    _horizontalGroupAction.addAction(&_sourcePluginPickerAction);

    _sourcePluginPickerAction.setFilterPluginTypes({ plugin::Type::VIEW });

    connect(&_sourcePluginPickerAction, &PluginPickerAction::pluginPicked, this, [this](plugin::Plugin* plugin) -> void {
        if (!plugin)
            return;

        if (_viewPluginSamplerAction)
            disconnect(_viewPluginSamplerAction, &ViewPluginSamplerAction::toolTipHtmlStringChanged, this, nullptr);

        _viewPluginSamplerAction = dynamic_cast<ViewPluginSamplerAction*>(plugin->findChildByPath("Sampler"));

        if (_viewPluginSamplerAction) {
            const auto updateHtmlText = [this]() -> void {
                _sampleContextWidget.setHtmlText(_viewPluginSamplerAction->getToolTipHtmlString());
            };

            updateHtmlText();

            connect(_viewPluginSamplerAction, &ViewPluginSamplerAction::toolTipHtmlStringChanged, this, updateHtmlText);
        }
    });
}

void SampleContextPlugin::init()
{
    auto layout = new QVBoxLayout();

    //layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(_horizontalGroupAction.createWidget(&getWidget()));
    layout->addWidget(&_sampleContextWidget, 1);

    getWidget().setLayout(layout);
}

SampleContextPluginFactory::SampleContextPluginFactory() :
    ViewPluginFactory()
{
}

QIcon SampleContextPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("microscope", color);
}

QUrl SampleContextPluginFactory::getRespositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

ViewPlugin* SampleContextPluginFactory::produce()
{
    return new SampleContextPlugin(this);
}
