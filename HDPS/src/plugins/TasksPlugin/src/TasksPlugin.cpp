// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.TasksPlugin")

using namespace hdps;

TasksPlugin::TasksPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _tasksAction(this, "Tasks")
{
}

void TasksPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    auto tasksWidget = _tasksAction.createWidget(&getWidget(), TasksAction::Toolbar | TasksAction::Overlay);

    tasksWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(tasksWidget);

    getWidget().setLayout(layout);
}

TasksPluginFactory::TasksPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon TasksPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("tasks", color);
}

ViewPlugin* TasksPluginFactory::produce()
{
    return new TasksPlugin(this);
}
