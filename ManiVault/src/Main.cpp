// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"
#include "private/NoProxyRectanglesFusionStyle.h"

#include <Application.h>
#include <ManiVaultVersion.h>
#include <ProjectMetaAction.h>

#include <models/ProjectsTreeModel.h>

#include <util/Icon.h>
#include <util/HardwareSpec.h>

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

    // Create a temporary core application to be able to read command line arguments without implicit interfacing with settings
    auto coreApplication = QSharedPointer<QCoreApplication>(new QCoreApplication(argc, argv));

    QCommandLineParser commandLineParser;

    commandLineParser.setApplicationDescription("Application for viewing and analyzing high-dimensional data");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption organizationNameOption({ "org_name", "organization_name" }, "Name of the organization", "organization_name", "BioVault");
    QCommandLineOption organizationDomainOption({ "org_dom", "organization_domain" }, "Domain of the organization", "organization_domain", "LUMC (LKEB) & TU Delft (CGV)");
    QCommandLineOption applicationNameOption({ "app_name", "application_name" }, "Name of the application", "application_name", "ManiVault");

    commandLineParser.addOption(organizationNameOption);
    commandLineParser.addOption(organizationDomainOption);
    commandLineParser.addOption(applicationNameOption);

    commandLineParser.process(QCoreApplication::arguments());

    // Remove the temporary application
    coreApplication.reset();

    QCoreApplication::setOrganizationName(commandLineParser.value("organization_name"));
    QCoreApplication::setOrganizationDomain(commandLineParser.value("organization_domain"));
    QCoreApplication::setApplicationName(commandLineParser.value("application_name"));
    
    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

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

    qDebug() << "Starting ManiVault" << MV_VERSION_STRING();

    Application application(argc, argv);

    Application::setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));

    Core core;

    application.setCore(&core);

    core.createManagers();

    if (settings().getTemporaryDirectoriesSettingsAction().getRemoveStaleTemporaryDirsAtStartupAction().isChecked()) {
        application.getTemporaryDirs().getTask().setParentTask(&application.getStartupTask());
        application.getTemporaryDirs().removeStale();
        
        settings().getTemporaryDirectoriesSettingsAction().getScanForStaleTemporaryDirectoriesAction().trigger();
    }

    const QString projectsJsonFileName{ "projects.json" };

    const auto hasProjectsJsonInAppDir = QFileInfo(QCoreApplication::applicationDirPath(), projectsJsonFileName).exists();

    auto& projectsTreeModel = const_cast<ProjectsTreeModel&>(mv::projects().getProjectsTreeModel());

    if (hasProjectsJsonInAppDir)
        projectsTreeModel.populateFromJsonFile(projectsJsonFileName);

    core.initialize();

    application.initialize();

    HardwareSpec::updateSystemHardwareSpecs();

    auto& loadGuiTask = application.getStartupTask().getLoadGuiTask();

    loadGuiTask.setSubtasks({ "Apply styles", "Create main window", "Initializing start page" });
    loadGuiTask.setRunning();

    loadGuiTask.setSubtaskStarted("Apply styles");

    application.setStyle(new NoProxyRectanglesFusionStyle);

    loadGuiTask.setSubtaskFinished("Apply styles");
	
	ModalTask::getGlobalHandler()->setEnabled(true);

    auto shouldShowSplashScreen = true;

    SplashScreenAction splashScreenAction(&application, true);

    if (application.shouldOpenProjectAtStartup()) {
        if (auto startupProjectMetaAction = mv::projects().getProjectMetaAction(application.getStartupProjectUrl().toLocalFile())) {
            try {
                application.getStartupTask().getLoadProjectTask().setEnabled(true, true);

                splashScreenAction.setProjectMetaAction(startupProjectMetaAction);
                splashScreenAction.fromVariantMap(startupProjectMetaAction->getSplashScreenAction().toVariantMap());

                application.setStartupProjectMetaAction(startupProjectMetaAction);

                if (startupProjectMetaAction->getReadOnlyAction().isChecked() && startupProjectMetaAction->getApplicationIconAction().getOverrideAction().isChecked())
                    application.setWindowIcon(startupProjectMetaAction->getApplicationIconAction().getIconPickerAction().getIcon());

                if (!splashScreenAction.getEnabledAction().isChecked())
                    shouldShowSplashScreen = false;
            	//else {
                //    splashScreenAction.addAlert(SplashScreenAction::Alert::warning(QString("\
                //        <b>%1</b> does not exist. \
                //        Provide an existing project file path when using <b>-p</b>/<b>--project</b> ManiVault<sup>&copy;</sup> command line parameters. \
                //    ").arg(startupProjectFilePath)));

                //    throw std::runtime_error("Project file not found");
                //}
            }
            catch (std::exception& e)
            {
                qDebug() << "Unable to load startup project:" << e.what();
            }
            catch (...)
            {
                qDebug() << "Unable to load startup project due to an unhandled exception";
            }
        }
    }

    if (shouldShowSplashScreen)
        splashScreenAction.getOpenAction().trigger();

    MainWindow mainWindow;

    loadGuiTask.setSubtaskStarted("Create main window");

    mainWindow.show();

    loadGuiTask.setSubtaskFinished("Create main window");

    return application.exec();
}
