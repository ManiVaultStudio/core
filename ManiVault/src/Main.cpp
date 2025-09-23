// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"
#include "private/NoProxyRectanglesFusionStyle.h"

#include <Application.h>
#include <ManiVaultVersion.h>

#include <models/ProjectsTreeModel.h>

#include <util/Icon.h>
#include <util/HardwareSpec.h>
#include <util/StandardPaths.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <QProxyStyle>
#include <QStyleFactory>
#include <QSurfaceFormat>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QFileInfo>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

int main(int argc, char *argv[])
{
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

    Application::setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));

    Core core;

    application.setCore(&core);

    core.createManagers();

    auto& splashScreenAction = application.getConfigurationAction().getBrandingConfigurationAction().getSplashScreenAction();

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

    loadGuiTask.setSubtaskFinished("Create main window");

    return Application::exec();
}
