// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingPlugin.h"

#include <Application.h>
#include <util/Miscellaneous.h>

#include <QVBoxLayout>

Q_PLUGIN_METADATA(IID "studio.manivault.LoggingPlugin")

using namespace mv;
using namespace mv::gui;

LoggingPlugin::LoggingPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _loggingWidget(nullptr)
{
    getLearningCenterAction().getPluginMetaData().setDescription("Shows <b>ManiVault</b> logging");
    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "LoggingPlugin" }));
}

void LoggingPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_loggingWidget);

    getWidget().setLayout(layout);
    getWidget().setWindowTitle("Logging view");
}

LoggingPluginFactory::LoggingPluginFactory() :
    ViewPluginFactory(true)
{
    setPreferredDockArea(DockAreaFlag::Bottom);
}

QIcon LoggingPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("scroll", color);
}

QUrl LoggingPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/LoggingPlugin/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/LoggingPlugin/README.md");
#endif
}

ViewPlugin* LoggingPluginFactory::produce()
{
    return new LoggingPlugin(this);
}
