// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"

#include <Application.h>
#include <ProjectMetaAction.h>
#include <BackgroundTask.h>
#include <ManiVaultVersion.h>

#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QTemporaryDir>

#include <iostream>

using namespace mv;
using namespace mv::util;
using namespace mv::gui;

class NoFocusProxyStyle : public QProxyStyle {
public:
    NoFocusProxyStyle(QStyle *baseStyle = 0) :
        QProxyStyle(baseStyle)
    {
    }

    void drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const {
        if (element == QStyle::PE_FrameFocusRect)
            return;

        QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
};

ProjectMetaAction* getStartupProjectMetaAction(const QString& startupProjectFilePath)
{
    try {
        const QString metaJsonFilePath("meta.json");

        QFileInfo extractFileInfo(Application::current()->getTemporaryDir().path(), metaJsonFilePath);

        Archiver archiver;

        QString extractedMetaJsonFilePath = "";

        archiver.extractSingleFile(startupProjectFilePath, metaJsonFilePath, extractFileInfo.absoluteFilePath());

        extractedMetaJsonFilePath = extractFileInfo.absoluteFilePath();

        if (!QFileInfo(extractedMetaJsonFilePath).exists())
            throw std::runtime_error("Unable to extract meta.json");

        return new ProjectMetaAction(extractedMetaJsonFilePath);
    }
    catch (std::exception& e)
    {
        qDebug() << "No meta data available, please re-save the project to solve the problem"  << e.what();
    }
    catch (...)
    {
        qDebug() << "No meta data available due to an unhandled problem, please re-save the project to solve the problem";
    }

    return nullptr;
}

int main(int argc, char *argv[])
{
    // Create a temporary core application to be able to read command line arguments without implicit interfacing with settings
    auto coreApplication = QSharedPointer<QCoreApplication>(new QCoreApplication(argc, argv));

    QCommandLineParser commandLineParser;

    commandLineParser.setApplicationDescription("Application for viewing and analyzing high-dimensional data");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption projectOption({ "p", "project" }, "File path of the project to load upon startup", "project");
    QCommandLineOption organizationNameOption({ "org_name", "organization_name" }, "Name of the organization", "organization_name", "BioVault");
    QCommandLineOption organizationDomainOption({ "org_dom", "organization_domain" }, "Domain of the organization", "organization_domain", "LUMC (LKEB) & TU Delft (CGV)");
    QCommandLineOption applicationNameOption({ "app_name", "application_name" }, "Name of the application", "application_name", "ManiVault");

    commandLineParser.addOption(projectOption);
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

#ifdef __APPLE__
    QSurfaceFormat defaultFormat;
    
    defaultFormat.setVersion(3, 3);
    defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
    defaultFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    
    QSurfaceFormat::setDefaultFormat(defaultFormat);
#endif

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    qDebug() << "Starting ManiVault" << QString("%1.%2").arg(QString::number(MV_VERSION_MAJOR), QString::number(MV_VERSION_MINOR));

    Application application(argc, argv);

    Core core;

    application.setCore(&core);

    core.createManagers();

    SplashScreenAction splashScreenAction(&application, false);

    if (commandLineParser.isSet("project")) {
        try {
            const auto startupProjectFilePath = commandLineParser.value("project");

            if (startupProjectFilePath.isEmpty())
                throw std::runtime_error("Project file path is empty");

            const auto startupProjectFileInfo = QFileInfo(startupProjectFilePath);

            if (startupProjectFileInfo.exists()) {
                qDebug() << "Loading startup project from" << startupProjectFilePath;

                //ModalTask::getGlobalHandler()->setEnabled(false);

                application.setStartupProjectFilePath(startupProjectFilePath);
                
                application.getStartupTask().getLoadProjectTask().setEnabled(true, true);

                auto projectMetaAction = getStartupProjectMetaAction(startupProjectFilePath);

                if (projectMetaAction != nullptr) {
                    splashScreenAction.setProjectMetaAction(projectMetaAction);
                    splashScreenAction.fromVariantMap(projectMetaAction->getSplashScreenAction().toVariantMap());

                    application.setStartupProjectMetaAction(projectMetaAction);
                }
                else {
                    splashScreenAction.addAlert(SplashScreenAction::Alert::info(QString("\
                        No project meta info found for project <b>%1</b>. \
                        Re-configure the project settings and save the project to solve the problem. \
                    ").arg(startupProjectFileInfo.fileName())));
                }
            }
            else {
                splashScreenAction.addAlert(SplashScreenAction::Alert::warning(QString("\
                    Unable to load <b>%1</b> at startup, the file does not exist. \
                    Provide an exisiting project file path when using <b>-p</b>/<b>--project</b> ManiVault<sup>&copy;</sup> command line parameters. \
                ").arg(startupProjectFilePath)));

                throw std::runtime_error("Project file not found");
            }
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
    
    splashScreenAction.getOpenAction().trigger();

    if (settings().getTemporaryDirectoriesSettingsAction().getRemoveStaleTemporaryDirsAtStartupAction().isChecked()) {
        application.getTemporaryDirs().getTask().setParentTask(&application.getStartupTask());
        application.getTemporaryDirs().removeStale();
        
        settings().getTemporaryDirectoriesSettingsAction().getScanForStaleTemporaryDirectoriesAction().trigger();
    }

    core.initialize();

    application.initialize();

    auto& loadGuiTask = application.getStartupTask().getLoadGuiTask();

    loadGuiTask.setSubtasks({ "Apply styles", "Create main window", "Initializing start page" });
    loadGuiTask.setRunning();

    loadGuiTask.setSubtaskStarted("Apply styles");

    application.setStyle(new NoFocusProxyStyle);

    QFile styleSheetFile(":/styles/default.qss");

    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    application.setStyleSheet(styleSheet);

    loadGuiTask.setSubtaskFinished("Apply styles");

    QIcon appIcon;

    appIcon.addFile(":/Icons/AppIcon32");
    appIcon.addFile(":/Icons/AppIcon64");
    appIcon.addFile(":/Icons/AppIcon128");
    appIcon.addFile(":/Icons/AppIcon256");
    appIcon.addFile(":/Icons/AppIcon512");
    appIcon.addFile(":/Icons/AppIcon1024");

    application.setWindowIcon(appIcon);

    loadGuiTask.setSubtaskStarted("Create main window");

    QCoreApplication::processEvents();

    MainWindow mainWindow;

    loadGuiTask.setSubtaskFinished("Create main window");

    QCoreApplication::processEvents();

    mainWindow.show();

    return application.exec();
}
