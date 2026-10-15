// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"
#include "private/NoProxyRectanglesFusionStyle.h"
#include "private/PassthroughBlobCodec.h"
#include "private/PassthroughBlobCodecFactory.h"
#include "private/ZstdBlobCodec.h"
#include "private/ZstdBlobCodecFactory.h"
#include "private/TaskflowWorkflowPlanExecutor.h"
#include "actions/SplashScreenAction.h"

#include <Application.h>
#include <ManiVaultVersion.h>

#include <models/ProjectsTreeModel.h>

#include <util/HardwareSpec.h>
#include <util/StandardPaths.h>
#include <util/BlobCodec.h>
#include <util/StyledIcon.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <QProxyStyle>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QFileInfo>
#include <QProcess>
#include <QTimer>
#include <QWebEnginePage>

namespace {

constexpr auto softwareWebEngineFallbackArgument = "--webengine-software-fallback";

bool hasSoftwareWebEngineFallbackArgument(int argc, char* argv[])
{
    for (int index = 0; index < argc; ++index) {
        if (QString::fromLocal8Bit(argv[index]) == QString::fromLatin1(softwareWebEngineFallbackArgument))
            return true;
    }

    return false;
}

void enableSoftwareWebEngineFallback()
{
    const QByteArray existingFlags = qgetenv("QTWEBENGINE_CHROMIUM_FLAGS");
    const QByteArray disableGpuFlag = "--disable-gpu";

    if (existingFlags.split(' ').contains(disableGpuFlag))
        return;

    QByteArray flags = existingFlags.trimmed();
    if (!flags.isEmpty())
        flags += ' ';
    flags += disableGpuFlag;
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", flags);
}

bool restartWithSoftwareWebEngineFallback(const QStringList& arguments)
{
    auto restartedArguments = arguments;
    if (!restartedArguments.contains(QString::fromLatin1(softwareWebEngineFallbackArgument)))
        restartedArguments << QString::fromLatin1(softwareWebEngineFallbackArgument);

    return QProcess::startDetached(QCoreApplication::applicationFilePath(), restartedArguments,
        QCoreApplication::applicationDirPath());
}

}

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

int main(int argc, char *argv[])
{
    const bool softwareWebEngineFallback = hasSoftwareWebEngineFallbackArgument(argc, argv);

    if (softwareWebEngineFallback)
        enableSoftwareWebEngineFallback();

    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    // Temporary application to be able to query application dir path
    auto tempApp = QSharedPointer<QCoreApplication>(new QCoreApplication(argc, argv));

    // Initialize application attributes (organization name, domain and application name)
    Application::initializeAttributes();

    // Destroy temporary application
    tempApp.reset();

#ifdef Q_OS_MAC
    QSurfaceFormat defaultFormat;
    
    defaultFormat.setVersion(3, 3);
    defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
    defaultFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    
    QSurfaceFormat::setDefaultFormat(defaultFormat);
#endif

#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    QSurfaceFormat format;

    format.setRenderableType(QSurfaceFormat::OpenGL);

    QSurfaceFormat::setDefaultFormat(format);
#endif

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    qDebug() << "Starting" << Application::applicationName();

    Application application(argc, argv);

    Application::setWorkflowPlanExecutor(std::make_unique<TaskflowWorkflowPlanExecutor>());

    codecRegistry().registerFactory(std::make_unique<PassthroughBlobCodecFactory>(&application));
    codecRegistry().registerFactory(std::make_unique<ZstdBlobCodecFactory>(&application));

    Core core;

    application.setCore(&core);

    core.createManagers();

    workflow::AbstractWorkflowPlanExecutor::installNotificationLinkHandler();

    auto& splashScreenAction = application.getConfigurationAction().getBrandingConfigurationAction().getSplashScreenAction();

    bool webEngineFallbackRestartRequested = false;
    QObject::connect(&splashScreenAction, &SplashScreenAction::webEngineRenderProcessTerminated,
        &application, [&](QWebEnginePage::RenderProcessTerminationStatus terminationStatus, int exitCode) {
            if (softwareWebEngineFallback || webEngineFallbackRestartRequested)
                return;

            webEngineFallbackRestartRequested = true;
            qWarning() << "Qt WebEngine renderer terminated during startup; restarting with GPU disabled"
                       << terminationStatus << exitCode;

            if (!restartWithSoftwareWebEngineFallback(application.arguments())) {
                qWarning() << "Unable to restart ManiVault with the Qt WebEngine software fallback";
                webEngineFallbackRestartRequested = false;
                return;
            }

            QCoreApplication::exit(0);
        });

    splashScreenAction.getOpenAction().trigger();

    if (settings().getTemporaryDirectoriesSettingsAction().getRemoveStaleTemporaryDirsAtStartupAction().isChecked()) {
        application.getTemporaryDirs().getTask().setParentTask(&application.getStartupTask());
        application.getTemporaryDirs().removeStale();
        
        settings().getTemporaryDirectoriesSettingsAction().getScanForStaleTemporaryDirectoriesAction().trigger();
    }

    const auto projectsJsonFilePath = QDir::cleanPath(StandardPaths::getCustomizationDirectory() + "/projects.json");
    const auto hasProjectsJsonFile  = QFileInfo(projectsJsonFilePath).exists();

    auto& projectsTreeModel = const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel());

    if (hasProjectsJsonFile)
        projectsTreeModel.populateFromJsonFile(projectsJsonFilePath);

    core.initialize();
    application.initialize();

    HardwareSpec::updateSystemHardwareSpecs();

    auto& loadGuiTask = application.getStartupTask().getLoadGuiTask();

    loadGuiTask.setSubtasks({ "Apply styles", "Create main window", "Initializing start page" });
    loadGuiTask.setRunning();

    loadGuiTask.setSubtaskStarted("Apply styles");

    Application::setStyle(new NoProxyRectanglesFusionStyle);

    loadGuiTask.setSubtaskFinished("Apply styles");
    
    ModalTask::getGlobalHandler()->setEnabled(true);

    MainWindow mainWindow;

    loadGuiTask.setSubtaskStarted("Create main window");

    mainWindow.show();
    mainWindow.initialize();

    if (softwareWebEngineFallback) {
        QTimer::singleShot(1500, &application, []() {
            mv::help().addNotification(
                "Web content compatibility mode",
                "ManiVault detected a graphics compatibility issue while initializing embedded web content and restarted successfully. "
                "Hardware acceleration has been disabled for <b>embedded web content only</b>; ManiVault's main rendering remains hardware accelerated. "
                "Updating your graphics driver may resolve the problem so hardware acceleration can be restored in a future session.",
                util::StyledIcon("circle-exclamation"));
        });
    }

    loadGuiTask.setSubtaskFinished("Create main window");

    return Application::exec();
}
