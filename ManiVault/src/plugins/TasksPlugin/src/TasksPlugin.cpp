// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPlugin.h"

#include <ModalTaskTester.h>

#include <Application.h>

#include <QMetaType>
#include <QMetaObject>

Q_PLUGIN_METADATA(IID "studio.manivault.TasksPlugin")

using namespace mv;
using namespace mv::util;

TasksPlugin::TasksPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _tasksAction(this, "Tasks")
{
    _filterModel.setSourceModel(&_model);

    _tasksAction.initialize(&_model, &_filterModel, "Task", [this](WidgetAction* action, QWidget* widget) -> void {
    }, false);

    AbstractTaskTester::registerTester("mv::ModalTaskTester");
    AbstractTaskTester::registerTester("mv::BackgroundTaskTester");
    AbstractTaskTester::registerTester("mv::ForegroundTaskTester");

    getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
    getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "TasksPlugin" }));
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
            const auto metaTypeName = modalTaskTestTypeAction->getCurrentText();
            const auto metaType     = QMetaType::fromName(metaTypeName.toLatin1());
            const auto metaObject   = metaType.metaObject();

            if (!metaObject)
                throw std::runtime_error(QString("Meta object type '%1' is not registered. Did you forget to register the tester correctly with the Qt meta object system?").arg(metaTypeName).toLatin1());

            auto metaObjectInstance = metaObject->newInstance(Q_ARG(QObject*, this), Q_ARG(QString, metaTypeName));
            auto taskTester         = dynamic_cast<AbstractTaskTester*>(metaObjectInstance);

            if (!taskTester)
                throw std::runtime_error(QString("Unable to create a new instance of type '%1'").arg(metaTypeName).toLatin1());
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
    setIconByName("tasks");

    getPluginMetadata().setDescription("For interacting with tasks");
    getPluginMetadata().setSummary("This system view plugin is for interacting with tasks (for debugging purposes only).");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
        { "T. Kroes", { "Lead software architect" }, { "LUMC" } }
    });
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
    });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl TasksPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return { "https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/TasksPlugin/README.md" };
#else
    return { "https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/TasksPlugin/README.md" };
#endif
}

QUrl TasksPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

ViewPlugin* TasksPluginFactory::produce()
{
    return new TasksPlugin(this);
}
