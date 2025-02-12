// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Application.h"
#include "CoreInterface.h"
#include "BackgroundTask.h"
#include "ForegroundTask.h"
#include "ManiVaultVersion.h"

#include "util/IconFonts.h"
#include "util/FontAwesome.h"
#include "util/Exception.h"

#include "actions/WidgetAction.h"
#include "actions/StatusBarAction.h"

#include <stdexcept>

#include <QDebug>
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
    _version(MV_VERSION_MAJOR, MV_VERSION_MINOR, MV_VERSION_PATCH, std::string(MV_VERSION_SUFFIX.data())),
    _serializationAborted(false),
    _startupProjectMetaAction(nullptr),
    _startupTask(nullptr),
    _temporaryDir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + QString("%1.%2").arg(Application::getName(), _id.mid(0, 6)))),
    _temporaryDirs(this),
    _lockFile(QDir::cleanPath(_temporaryDir.path() + QDir::separator() + "app.lock"))
{
    _lockFile.lock();

    qDebug() << "Initializing icon fonts";

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14, { ":/IconFonts/FontAwesomeBrandsRegular-5.14.otf" }, false, "FontAwesomeBrands")));

#if defined(_WIN32) || defined(_WIN64)
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14, { ":/IconFonts/FontAwesomeRegular-5.14.otf" }, false, "FontAwesomeRegular")));
#endif

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14, { ":/IconFonts/FontAwesomeSolid-5.14.otf" }, true, "FontAwesome")));

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 4, { ":/IconFonts/FontAwesomeBrandsRegular-6.4.otf" }, false, "FontAwesomeBrands")));

#if defined(_WIN32) || defined(_WIN64)
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 4, { ":/IconFonts/FontAwesomeRegular-6.4.otf" }, false, "FontAwesomeRegular")));
#endif

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 4, { ":/IconFonts/FontAwesomeSolid-6.4.otf" }, false, "FontAwesome")));

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 5, { ":/IconFonts/FontAwesomeBrandsRegular-6.5.otf" }, false, "FontAwesomeBrands")));

#if defined(_WIN32) || defined(_WIN64)
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 5, { ":/IconFonts/FontAwesomeRegular-6.5.otf" }, false, "FontAwesomeRegular")));
#endif

    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(6, 5, { ":/IconFonts/FontAwesomeSolid-6.5.otf" }, false, "FontAwesome")));

    connect(Application::current(), &Application::coreManagersCreated, this, [this](CoreInterface* core) {
        _startupTask = new ApplicationStartupTask(this, "Load ManiVault");

        auto& temporaryDirsTask = _temporaryDirs.getTask();

        temporaryDirsTask.addToTaskManager();
    });

    connect(Application::current(), &Application::coreInitialized, this, [this](CoreInterface* core) {
        BackgroundTask::createHandler(Application::current());
        ForegroundTask::createHandler(Application::current());
        ModalTask::createHandler(Application::current());
    });

    _currentPalette = palette();
    
    
}

Application::~Application()
{
    _core = nullptr;
}

bool Application::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange) {
        auto currentPalette = QGuiApplication::palette();

        if (currentPalette != _currentPalette) {
            emit paletteChanged(_currentPalette);

            _currentPalette = currentPalette;
            
            for (auto widget : QApplication::allWidgets())
				widget->repaint();
        }
    }

    return QApplication::event(event);
}

Application* Application::current()
{
    auto applicationInstance = instance();
    auto maniVaultApplication = dynamic_cast<Application*>(applicationInstance);

    return maniVaultApplication;
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
    if (current() == nullptr)
        return nullptr;

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

QString Application::getAbout()
{
    return QString("<p>%3 is a flexible and extensible visual analytics framework for high-dimensional data.<br>"
        "For more information, please visit: <a href=\"http://%2/\">%2</a> </p>"
        "This software is licensed under the GNU Lesser General Public License v3.0.<br>"
        "Copyright (C) %1 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)"
    ).arg(QStringLiteral("2023"), QStringLiteral("github.com/ManiVaultStudio"), getName());
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

const QTemporaryDir& Application::getTemporaryDir() const
{
    return _temporaryDir;
}

Application::TemporaryDirs& Application::getTemporaryDirs()
{
    return _temporaryDirs;
}

Application::TemporaryDirs::TemporaryDirs(QObject* parent) :
    QObject(parent),
    _task(this, "Remove stale ManiVault temporary directories")
{
    _task.setGuiScopes({ Task::GuiScope::Background });
}

QStringList Application::TemporaryDirs::getStale()
{
    QStringList staleTemporaryDirectories;

    auto temporaryDir = QDir(QDir::tempPath());

    temporaryDir.setFilter(QDir::Dirs);
    temporaryDir.setNameFilters({ "ManiVault.*" });

    const auto sessions = temporaryDir.entryList();

    for (const auto& session : sessions) {
        const auto lockInfoFilePath         = QDir::cleanPath(QDir::tempPath() + QDir::separator() + session + QDir::separator() + "app.lock");
        const auto sessionSegments          = session.split(".");
        const auto isCurrentApplication     = sessionSegments.count() > 2 && Application::current()->getId().startsWith(sessionSegments[1]);
        const auto staleTemporaryDirectory  = QDir::cleanPath(QDir::tempPath() + QDir::separator() + session);

        if (isCurrentApplication)
            continue;

        if (QFileInfo(lockInfoFilePath).exists()) {
            QLockFile lockFile(lockInfoFilePath);

            if (lockFile.tryLock(150)) {
                staleTemporaryDirectories << staleTemporaryDirectory;

                lockFile.unlock();
            }
        }
        else {
            staleTemporaryDirectories << staleTemporaryDirectory;
        }
    }

    return staleTemporaryDirectories;
}

void Application::TemporaryDirs::removeStale(const QStringList& stale /*= QStringList()*/)
{
    const auto staleTemporaryDirectories = getStale();

    qDebug() << "Found" << staleTemporaryDirectories.count() << QString("stale temporary ManiVault director%1 eligible for removal").arg(staleTemporaryDirectories.count() == 1 ? "y" : "ies");

    int numberOfRemovedSessions = 0;

    QStringList selectedStaleTemporaryDirs;

    if (!stale.isEmpty()) {
        for (const auto& staleTemporaryDirectory : staleTemporaryDirectories)
            if (stale.contains(staleTemporaryDirectory))
                selectedStaleTemporaryDirs << staleTemporaryDirectory;
    }
    else {
        selectedStaleTemporaryDirs = staleTemporaryDirectories;
    }

    _task.setSubtasks(selectedStaleTemporaryDirs);
    _task.setRunning();

    processEvents();

    for (const auto& selectedStaleTemporaryDir : selectedStaleTemporaryDirs) {
        try
        {
            const auto dirName = selectedStaleTemporaryDir.split("/").last();

            _task.setSubtaskStarted(selectedStaleTemporaryDir, QString("Removing %1").arg(dirName));
            {
                qDebug() << "Removing" << selectedStaleTemporaryDir;

                QDir sessionDir(selectedStaleTemporaryDir);

                if (!sessionDir.removeRecursively())
                    throw std::runtime_error(QString("Unable to remove %1").arg(selectedStaleTemporaryDir).toStdString());

                ++numberOfRemovedSessions;
            }
            _task.setSubtaskFinished(selectedStaleTemporaryDir, QString("Removed %1").arg(dirName));

            processEvents();
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to remove the ManiVault application temporary directory", e);
        }
        catch (...) {
            exceptionMessageBox("Unable to remove the ManiVault application temporary directory");
        }
    }

    qDebug() << "Removed" << numberOfRemovedSessions << QString("ManiVault application temporary director%1").arg(numberOfRemovedSessions == 1 ? "y" : "ies");

    _task.setFinished();
}

}
