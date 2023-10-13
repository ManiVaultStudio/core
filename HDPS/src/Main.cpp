// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"

#include <Application.h>
#include <ProjectMetaAction.h>

#include <ModalTask.h>
#include <ModalTaskHandler.h>

#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QTemporaryDir>

using namespace hdps;
using namespace hdps::util;

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
        QTemporaryDir temporaryDir;

        const QString metaJsonFilePath("meta.json");

        QFileInfo extractFileInfo(temporaryDir.path(), metaJsonFilePath);

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
    QCoreApplication::setOrganizationName("BioVault");
    QCoreApplication::setOrganizationDomain("LUMC (LKEB) & TU Delft (CGV)");
    QCoreApplication::setApplicationName("ManiVault");
    
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

    Application application(argc, argv);

    Core core;

    application.setCore(&core);

    core.createManagers();

    QCommandLineParser commandLineParser;

    commandLineParser.setApplicationDescription("Application for viewing and analyzing high-dimensional data");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption projectOption({ "p", "project" }, "File path of the project to load upon startup", "project");

    commandLineParser.addOption(projectOption);
    commandLineParser.process(QCoreApplication::arguments());
    
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
                
                application.getStartupTask().getLoadProjectTask().setEnabled(true);

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

    core.initialize();

    QCoreApplication::processEvents();

    application.initialize();

    application.setStyle(new NoFocusProxyStyle);

    QFile styleSheetFile(":/styles/default.qss");

    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    application.setStyleSheet(styleSheet);

    QIcon appIcon;

    appIcon.addFile(":/Icons/AppIcon32");
    appIcon.addFile(":/Icons/AppIcon64");
    appIcon.addFile(":/Icons/AppIcon128");
    appIcon.addFile(":/Icons/AppIcon256");
    appIcon.addFile(":/Icons/AppIcon512");
    appIcon.addFile(":/Icons/AppIcon1024");

    application.setWindowIcon(appIcon);

    MainWindow mainWindow;

    mainWindow.show();

    return application.exec();
}
