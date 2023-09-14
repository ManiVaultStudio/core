// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPlugin.h"

#include <Application.h>

#include <actions/GroupAction.h>
#include <actions/OptionAction.h>
#include <actions/TriggerAction.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.TasksPlugin")

using namespace hdps;

#ifdef _DEBUG

QMap<TasksPlugin::TestMode, QString> TasksPlugin::testModeNames = QMap<TasksPlugin::TestMode, QString>({
    { TasksPlugin::TestMode::ModalRunningIndeterminate, "ModalRunningIndeterminate" },
    { TasksPlugin::TestMode::ModalSubtasks, "ModalSubtasks" }
});

#endif

TasksPlugin::TasksPlugin(const PluginFactory* factory) :
    ViewPlugin(factory)//,
    //_tasksAction(this, "Tasks")
{
}

void TasksPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    //auto tasksWidget = _tasksAction.createWidget(&getWidget(), TasksAction::Toolbar | TasksAction::Overlay);

    //tasksWidget->layout()->setContentsMargins(0, 0, 0, 0);

    //layout->addWidget(tasksWidget);

    getWidget().setLayout(layout);

#ifdef _DEBUG
    addTestSuite();
#endif
}

#ifdef _DEBUG

void TasksPlugin::addTestSuite()
{
    auto testModalTaskGroupAction   = new GroupAction(this, "Test Modal Task");
    auto modalTaskTestTypeAction    = new OptionAction(this, "Modal Task Test Type", testModeNames.values(), testModeNames.values().first());
    auto modalTaskStartTestAction   = new TriggerAction(this, "Start Test");

    modalTaskTestTypeAction->setPlaceHolderString("<Pick Type>");

    testModalTaskGroupAction->addAction(modalTaskTestTypeAction);
    testModalTaskGroupAction->addAction(modalTaskStartTestAction);

    getWidget().layout()->addWidget(testModalTaskGroupAction->createWidget(&getWidget()));

    connect(modalTaskStartTestAction, &TriggerAction::triggered, this, [this, modalTaskTestTypeAction]() -> void {
        switch (static_cast<TestMode>(modalTaskTestTypeAction->getCurrentIndex()))
        {
            case TestMode::ModalRunningIndeterminate:
                new ModalRunningIndeterminateTester(this, modalTaskTestTypeAction->getCurrentText());
                break;

            case TestMode::ModalSubtasks:
                new ModalRunningIndeterminateTester(this, modalTaskTestTypeAction->getCurrentText());
                break;
        }
    });
}
#endif

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
