// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Application.h"
#include "CoreInterface.h"
#include "BackgroundTask.h"
#include "ForegroundTask.h"
#include "ManiVaultVersion.h"

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

QList<Application::CursorShapeCount> Application::cursorOverridesCount;

Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    Serializable("Application"),
    _core(nullptr),
    _version(MV_VERSION_MAJOR, MV_VERSION_MINOR, MV_VERSION_PATCH, std::string(MV_VERSION_SUFFIX.data())),
    _serializationAborted(false),
    _startupProjectMetaAction(nullptr),
    _startupTask(nullptr),
    _temporaryDir(QDir::cleanPath(QDir::tempPath() + QDir::separator() + QString("%1.%2").arg(Application::getName(), getId().mid(0, 6)))),
    _temporaryDirs(this),
    _lockFile(QDir::cleanPath(_temporaryDir.path() + QDir::separator() + "app.lock"))
{
    _lockFile.lock();

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
}

Application::~Application()
{
    _core = nullptr;
}

Application* Application::current()
{
    auto applicationInstance = instance();
    auto maniVaultApplication = dynamic_cast<Application*>(applicationInstance);

    return maniVaultApplication;
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
    return QString("%3 is a flexible and extensible visual analytics framework for high-dimensional data.<br> <br>"
        "For more information, please visit: <br>"
        "Webpage: <a href=\"https://%5/\">%5</a> <br>"
        "Source: <a href=\"https://%2/\">%2</a> <br> <br>"
        "%3 Core %4 <br> <br>"
        "This software is licensed under the GNU Lesser General Public License v3.0.<br>"
        "Copyright (C) %1 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)"
    ).arg(
        /*1: year*/ QStringLiteral("2023"),
        /*2: source*/ QStringLiteral("github.com/ManiVaultStudio"),
        /*3: name*/ getName(),
        /*4: version*/ QString::fromStdString(Application::current()->getVersion().getVersionString()),
        /*5: webpage*/ QStringLiteral("manivault.studio")
    );
}

QUrl Application::getStartupProjectUrl() const
{
    return _startupProjectUrl;
}

void Application::setStartupProjectUrl(const QUrl& startupProjectUrl)
{
    _startupProjectUrl = startupProjectUrl;
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
    if (_startupProjectUrl.isEmpty())
        return false;

    if (_startupProjectUrl.isLocalFile() && !QFileInfo(_startupProjectUrl.toLocalFile()).exists())
        return false;

    return true;
}

bool Application::hasSetting(const QString& path) const
{
    return _settings.contains(path);
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

const QTemporaryDir& Application::getTemporaryDir() const
{
    return _temporaryDir;
}

Application::TemporaryDirs& Application::getTemporaryDirs()
{
    return _temporaryDirs;
}

std::int32_t Application::requestOverrideCursor(Qt::CursorShape cursorShape)
{
    auto it = std::find_if(cursorOverridesCount.begin(), cursorOverridesCount.end(), [&cursorShape](const CursorShapeCount& cursorShapeCount) {
	    return cursorShapeCount.shape == cursorShape;
    });

    if (it != cursorOverridesCount.end())
        it->count++;
    else
        cursorOverridesCount.emplace_back(CursorShapeCount{ cursorShape, 1 });

    std::sort(cursorOverridesCount.begin(), cursorOverridesCount.end(), [cursorShape](auto& cursorShapeCountLhs, auto& cursorShapeCountRhs) -> bool {
        if (cursorShapeCountRhs.shape == cursorShape)
            return true;

    	return cursorShapeCountRhs.count > cursorShapeCountLhs.count;
    });

    setOverrideCursor(cursorOverridesCount.first().shape);
    processEvents();

    return cursorOverridesCount.first().count;
}

std::int32_t Application::requestRemoveOverrideCursor(Qt::CursorShape cursorShape, bool all)
{
    if (!current()) {
        qWarning() << "Application::requestRemoveOverrideCursor failed: No current application instance";
        return 0;
    }

    auto it = std::find_if(cursorOverridesCount.begin(), cursorOverridesCount.end(), [&cursorShape](const CursorShapeCount& cursorShapeCount) {
        return cursorShapeCount.shape == cursorShape;
	});

    if (it != cursorOverridesCount.end()) {
        if (all)
            it->count = 0;
        else
            it->count--;

        if (it->count <= 0)
            cursorOverridesCount.erase(it);
    } else {
        qWarning() << "Application::requestRemoveOverrideCursor failed: shape is not overriden";

        if (cursorOverridesCount.empty()) {

        	while (QApplication::overrideCursor())
                QApplication::restoreOverrideCursor();

            return 0;
        }
    }

    std::sort(cursorOverridesCount.begin(), cursorOverridesCount.end(), [](auto& cursorShapeCountLhs, auto& cursorShapeCountRhs) -> bool {
        return cursorShapeCountRhs.count > cursorShapeCountLhs.count;
    });

    if (cursorOverridesCount.empty()) {

    	while (QApplication::overrideCursor())
            QApplication::restoreOverrideCursor();

        return 0;
    }

	setOverrideCursor(cursorOverridesCount.first().shape);
    processEvents();

    return cursorOverridesCount.first().count;
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
