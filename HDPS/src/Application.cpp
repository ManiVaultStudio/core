// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Application.h"
#include "CoreInterface.h"
#include "BackgroundTask.h"
#include "ForegroundTask.h"
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
    _startupProjectMetaAction(nullptr),
    _startupTask(nullptr)
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14, {
            //":/IconFonts/FontAwesomeBrandsRegular-5.14.otf",
            //":/IconFonts/FontAwesomeRegular-5.14.otf",
            ":/IconFonts/FontAwesomeSolid-5.14.otf"
    }, true)));

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 4, {
        //":/IconFonts/FontAwesomeBrandsRegular-6.4.otf",
        //":/IconFonts/FontAwesomeRegular-6.4.otf",
        ":/IconFonts/FontAwesomeSolid-6.4.otf"
    })));

    connect(Application::current(), &Application::coreManagersCreated, this, [this](CoreInterface* core) {
        _startupTask = new ApplicationStartupTask(this, "Load ManiVault");
    });

    connect(Application::current(), &Application::coreInitialized, this, [this](CoreInterface* core) {
        BackgroundTask::createHandler(Application::current());
        ForegroundTask::createHandler(Application::current());
        ModalTask::createHandler(Application::current());
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

QMainWindow* Application::getMainWindow()
{
    foreach(QWidget* widget, qApp->topLevelWidgets())
        if (auto mainWindow = qobject_cast<QMainWindow*>(widget))
            return mainWindow;

    return nullptr;
}

ApplicationStartupTask& Application::getStartupTask()
{
    return *_startupTask;
}

}
