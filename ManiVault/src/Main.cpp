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

#include <Application.h>
#include <ManiVaultVersion.h>

#include <models/ProjectsTreeModel.h>

#include <util/HardwareSpec.h>
#include <util/StandardPaths.h>
#include <util/BlobCodec.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <QProxyStyle>
#include <QStyleFactory>
#include <QSurfaceFormat>
#ifndef __EMSCRIPTEN__
#include <QQuickWindow>
#endif
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QFileInfo>

#ifdef __EMSCRIPTEN__
#include <QtPlugin>
Q_IMPORT_PLUGIN(DataHierarchyPluginFactory)
Q_IMPORT_PLUGIN(PointDataFactory)
#ifdef MV_WASM_SCATTERPLOT_PLUGIN
Q_IMPORT_PLUGIN(ScatterplotPluginFactory)
#endif
#endif

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

int main(int argc, char *argv[])
{
#ifndef __EMSCRIPTEN__
    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
#endif

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

#ifndef __EMSCRIPTEN__
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
#endif

    qDebug() << "Starting" << Application::applicationName();

    Application application(argc, argv);

    Application::setWorkflowPlanExecutor(std::make_unique<TaskflowWorkflowPlanExecutor>());

    codecRegistry().registerFactory(std::make_unique<PassthroughBlobCodecFactory>(&application));
    codecRegistry().registerFactory(std::make_unique<ZstdBlobCodecFactory>(&application));

    Core core;

    application.setCore(&core);

    core.createManagers();

    workflow::AbstractWorkflowPlanExecutor::installNotificationLinkHandler();

#ifdef __EMSCRIPTEN__
    // WorkspaceManager needs a top-level widget while the core managers are
    // initialized. On desktop the splash screen fulfills that role.
    MainWindow mainWindow;
#else
    auto& splashScreenAction = application.getConfigurationAction().getBrandingConfigurationAction().getSplashScreenAction();

    splashScreenAction.getOpenAction().trigger();
#endif

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

#ifndef __EMSCRIPTEN__
    HardwareSpec::updateSystemHardwareSpecs();
#endif

    auto& loadGuiTask = application.getStartupTask().getLoadGuiTask();

    loadGuiTask.setSubtasks({ "Apply styles", "Create main window", "Initializing start page" });
    loadGuiTask.setRunning();

    loadGuiTask.setSubtaskStarted("Apply styles");

    Application::setStyle(new NoProxyRectanglesFusionStyle);

    loadGuiTask.setSubtaskFinished("Apply styles");
    
    ModalTask::getGlobalHandler()->setEnabled(true);

#ifndef __EMSCRIPTEN__
    MainWindow mainWindow;
#endif

    loadGuiTask.setSubtaskStarted("Create main window");

    mainWindow.show();
    mainWindow.initialize();

#ifdef __EMSCRIPTEN__
    // Create a project/workspace and show the statically linked proof plugin.
    mv::projects().newBlankProject();
    mv::plugins().requestViewPlugin(QStringLiteral("Data hierarchy"));
#ifdef MV_WASM_SCATTERPLOT_PLUGIN
    mv::plugins().requestViewPlugin(QStringLiteral("Scatterplot View"));
#endif
#endif

    loadGuiTask.setSubtaskFinished("Create main window");

    return Application::exec();
}
