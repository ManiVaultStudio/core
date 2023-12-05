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
#include <QUuid>
#include <QDir>

using namespace mv::gui;
using namespace mv::util;

namespace mv {

Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _id(QUuid::createUuid().toString(QUuid::WithoutBraces)),
    _core(nullptr),
    _version({ 0, 9 }),
    _iconFonts(),
    _settings(),
    _serializationTemporaryDirectory(),
    _serializationAborted(false),
    _logger(),
    _startupProjectFilePath(),
    _startupProjectMetaAction(nullptr),
    _startupTask(nullptr),
    _temporaryDir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + QString("%1.%2").arg(Application::getName(), _id.mid(0, 6)))),
    _lockFile(QDir::cleanPath(_temporaryDir.path() + QDir::separator() + "app.lock"))
{
    _lockFile.lock();

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
        auto applicationInstance    = instance();
        auto maniVaultApplication   = dynamic_cast<Application*>(applicationInstance);

        if (maniVaultApplication == nullptr)
            throw std::runtime_error("Current application instance is not a ManiVault application");

        return maniVaultApplication;
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

mv::CoreInterface* Application::getCore()
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

        connect(core, &CoreInterface::aboutToBeInitialized, this, [this]() -> void { emit coreAboutToBeInitialized(_core); });
        connect(core, &CoreInterface::initialized, this, [this]() -> void { emit coreInitialized(_core); });
        connect(core, &CoreInterface::managersCreated, this, [this]() -> void { emit coreManagersCreated(_core); });
    }
    else {
        qDebug() << "Cannot assign core to application, core is already assigned";
    }
}

mv::CoreInterface* Application::core()
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

QString Application::getId() const
{
    return _id;
}

void Application::cleanTemporaryDirectory()
{
    auto temporaryDir = QDir(QDir::tempPath());

    temporaryDir.setFilter(QDir::Dirs);
    temporaryDir.setNameFilters({ "ManiVault.*" });

    const auto sessions = temporaryDir.entryList();

    qDebug() << "Found" << sessions.count() << "candidate temporary ManiVault directories for removal";

    for (const auto& session : sessions) {
        QLockFile lockFile(QDir::cleanPath(QDir::tempPath() + QDir::separator() + session + QDir::separator() + "app.lock"));

        if (lockFile.tryLock(150)) {
            try
            {
                qDebug() << "Removing" << session;

                QDir sessionDir(session);

                sessionDir.removeRecursively();
            }
            catch (std::exception& e)
            {
                exceptionMessageBox("Unable to remove the ManiVault application session", e);
            }
            catch (...) {
                exceptionMessageBox("Unable to remove the ManiVault application session");
            }

            lockFile.unlock();
        }
        else {
            qDebug() << session << "is locked so it cannot be removed at this point. Close the application or restart the OS to remove it.";
        }
    }
}

const QTemporaryDir& Application::getTemporaryDir() const
{
    return _temporaryDir;
}

}
