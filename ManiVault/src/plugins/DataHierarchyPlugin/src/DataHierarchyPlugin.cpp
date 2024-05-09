// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")

using namespace mv;
using namespace mv::gui;

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataHierarchyWidget(nullptr)
{
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
}

QIcon DataHierarchyPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sitemap", color);
}

QUrl DataHierarchyPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef _DEBUG
    return QUrl(QString("%1/blob/feature/learning_center/ManiVault/src/plugins/DataHierarchyPlugin/README.md").arg(getGitHubRespositoryUrl().path()));
#else
    return QUrl(QString("%1/blob/master/ManiVault/src/plugins/DataHierarchyPlugin/README.md").arg(getGitHubRespositoryUrl().path()));
#endif
}

QUrl DataHierarchyPluginFactory::getGitHubRespositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}
