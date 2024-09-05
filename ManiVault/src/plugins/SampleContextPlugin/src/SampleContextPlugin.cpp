// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "SampleContextPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.SampleContextPlugin")

using namespace mv;

SampleContextPlugin::SampleContextPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _sampleContextWidget(this, nullptr)
{
}

void SampleContextPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_sampleContextWidget);

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
