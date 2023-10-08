// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Application.h"
#include "CoreInterface.h"
#include "BackgroundTask.h"
#include "AbstractManager.h"

#include "util/IconFonts.h"
#include "util/FontAwesome.h"
#include "util/Exception.h"

#include "actions/WidgetAction.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>
#include <QMainWindow>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _core(nullptr),
    _version({ 1, 0 }),
    _iconFonts(),
    _settings(),
    _serializationTemporaryDirectory(),
    _serializationAborted(false),
    _logger(),
    _startupProjectFilePath(),
    _startupProjectMetaAction(nullptr)
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14, {
            ":/IconFonts/FontAwesomeBrandsRegular-5.14.otf",
            ":/IconFonts/FontAwesomeRegular-5.14.otf",
            ":/IconFonts/FontAwesomeSolid-5.14.otf"
    }, true)));

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 4, {
        ":/IconFonts/FontAwesomeBrandsRegular-6.4.otf",
        ":/IconFonts/FontAwesomeRegular-6.4.otf",
        ":/IconFonts/FontAwesomeSolid-6.4.otf"
    })));

    connect(this, &Application::coreManagersCreated, this, [this](CoreInterface* core) {
        _tasks << new Task(this, "Loading");
        _tasks << new Task(this, "Loading Core");
        _tasks << new Task(this, "Loading Core Managers");
        _tasks << new Task(this, "Loading GUI");
        _tasks << new Task(this, "Loading Project");
        _tasks << new Task(this, "Loading project data");
        _tasks << new Task(this, "Loading project workspace");
        _tasks << new BackgroundTask(this, "Overall Background");

        for (auto task : _tasks)
            task->setMayKill(false);

        getTask(TaskType::LoadApplicationCore)->setParentTask(getTask(TaskType::LoadApplication));
        getTask(TaskType::LoadApplicationCoreManagers)->setParentTask(getTask(TaskType::LoadApplicationCore));
        getTask(TaskType::LoadApplicationGUI)->setParentTask(getTask(TaskType::LoadApplication));
        getTask(TaskType::LoadProject)->setParentTask(getTask(TaskType::LoadApplication));
        getTask(TaskType::LoadProjectData)->setParentTask(getTask(TaskType::LoadProject));
        getTask(TaskType::LoadProjectWorkspace)->setParentTask(getTask(TaskType::LoadProject));

        getTask(TaskType::LoadProject)->setEnabled(false);
    });
}

Application* Application::current()
{
    try
    {
        auto hdpsApplication = dynamic_cast<Application*>(instance());

        if (hdpsApplication == nullptr)
            throw std::runtime_error("Current application instance is not a ManiVault application");

        return hdpsApplication;
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An application error occurred", e.what());
        return nullptr;
    }
}

const IconFont& Application::getIconFont(const QString& name, const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    return current()->_iconFonts.getIconFont(name, majorVersion, minorVersion);
}

hdps::CoreInterface* Application::getCore()
{
    return _core;
}

void Application::setCore(CoreInterface* core)
{
    Q_ASSERT(core != nullptr);

    if (core == nullptr)
        return;

    if (_core == nullptr) {
        _core = core;

        emit coreAssigned(_core);

        connect(core, &CoreInterface::aboutToBeInitialized, this, [this]() -> void { emit coreAboutToBeInitialized(_core); });
        connect(core, &CoreInterface::initialized, this, [this]() -> void { emit coreInitialized(_core); });
        connect(core, &CoreInterface::managersCreated, this, [this]() -> void { emit coreManagersCreated(_core); });
    }
    else {
        qDebug() << "Cannot assign core to application, core is already assigned";
    }
}

hdps::CoreInterface* Application::core()
{
    return current()->getCore();
}

util::Version Application::getVersion() const
{
    return _version;
}

QString Application::getName()
{
    return "ManiVault";
}

QString Application::getStartupProjectFilePath() const
{
    return _startupProjectFilePath;
}

void Application::setStartupProjectFilePath(const QString& startupProjectFilePath)
{
    _startupProjectFilePath = startupProjectFilePath;
}

ProjectMetaAction* Application::getStartupProjectMetaAction()
{
    return _startupProjectMetaAction;
}

void Application::setStartupProjectMetaAction(ProjectMetaAction* projectMetaAction)
{
    _startupProjectMetaAction = projectMetaAction;
}

bool Application::shouldOpenProjectAtStartup() const
{
    return !_startupProjectFilePath.isEmpty() && QFileInfo(_startupProjectFilePath).exists();
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

Logger& Application::getLogger()
{
    return current()->_logger;
}

QString Application::getSerializationTemporaryDirectory()
{
    return current()->_serializationTemporaryDirectory;
}

void Application::setSerializationTemporaryDirectory(const QString& serializationTemporaryDirectory)
{
    current()->_serializationTemporaryDirectory = serializationTemporaryDirectory;
}

bool Application::isSerializationAborted()
{
    return current()->_serializationAborted;
}

void Application::setSerializationAborted(bool serializationAborted)
{
    current()->_serializationAborted = serializationAborted;
}

void Application::initialize()
{
    _logger.initialize();
}

Task* Application::getTask(const TaskType& taskType)
{
    return _tasks[static_cast<int>(taskType)];
}

QMainWindow* Application::getMainWindow()
{
    foreach(QWidget* widget, qApp->topLevelWidgets())
        if (auto mainWindow = qobject_cast<QMainWindow*>(widget))
            return mainWindow;

    return nullptr;
}

}
