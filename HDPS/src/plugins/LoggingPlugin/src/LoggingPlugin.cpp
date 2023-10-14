// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingPlugin.h"

#include <Application.h>

#include <QVBoxLayout>

Q_PLUGIN_METADATA(IID "nl.BioVault.LoggingPlugin")

using namespace mv;
using namespace mv::gui;

LoggingPlugin::LoggingPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _loggingWidget(nullptr)
{
}

void LoggingPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_loggingWidget);

    getWidget().setLayout(layout);
    getWidget().setWindowTitle("Logging");
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

ViewPlugin* LoggingPluginFactory::produce()
{
    return new LoggingPlugin(this);
}
