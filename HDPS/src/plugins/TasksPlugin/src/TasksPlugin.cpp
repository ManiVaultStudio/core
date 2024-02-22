// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPlugin.h"

#include <ModalTaskTester.h>

#include <Application.h>

#include <actions/GroupAction.h>
#include <actions/OptionAction.h>
#include <actions/TriggerAction.h>

Q_PLUGIN_METADATA(IID "NL.ManiVault.TasksPlugin")

using namespace mv;
using namespace mv::util;

TasksPlugin::TasksPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _tasksAction(this, "Tasks")
{
    AbstractTaskTester::registerTester("mv::ModalTaskTester");
    AbstractTaskTester::registerTester("mv::BackgroundTaskTester");
    AbstractTaskTester::registerTester("mv::ForegroundTaskTester");
}

void TasksPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    auto tasksWidget = _tasksAction.createWidget(&getWidget());

    tasksWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(tasksWidget);

    getWidget().setLayout(layout);

#ifdef TEST_SUITE
    addTestSuite();
#endif
}

#ifdef TEST_SUITE

void TasksPlugin::addTestSuite()
{
    auto testModalTaskGroupAction   = new GroupAction(this, "Test Modal Task");
    auto modalTaskTestTypeAction    = new OptionAction(this, "Task Test Type", AbstractTaskTester::getTesterNames(), AbstractTaskTester::getTesterNames().first());
    auto modalTaskStartTestAction   = new TriggerAction(this, "Start Test");

    modalTaskTestTypeAction->setPlaceHolderString("<Pick Type>");

    testModalTaskGroupAction->addAction(modalTaskTestTypeAction);
    testModalTaskGroupAction->addAction(modalTaskStartTestAction);

    getWidget().layout()->addWidget(testModalTaskGroupAction->createWidget(&getWidget()));

    connect(modalTaskStartTestAction, &TriggerAction::triggered, this, [this, modalTaskTestTypeAction]() -> void {
        try {
            const auto metaType     = modalTaskTestTypeAction->getCurrentText();
            const auto metaTypeId   = QMetaType::type(metaType.toLatin1());
            const auto metaObject   = QMetaType::metaObjectForType(metaTypeId);

            if (!metaObject)
                throw std::runtime_error(QString("Meta object type '%1' is not registered. Did you forget to register the tester correctly with the Qt meta object system?").arg(metaType).toLatin1());

            auto metaObjectInstance = metaObject->newInstance(Q_ARG(QObject*, this), Q_ARG(QString, metaType));
            auto taskTester         = dynamic_cast<AbstractTaskTester*>(metaObjectInstance);

            if (!taskTester)
                throw std::runtime_error(QString("Unable to create a new instance of type '%1'").arg(metaType).toLatin1());
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to create task tester:", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to create task tester:");
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
