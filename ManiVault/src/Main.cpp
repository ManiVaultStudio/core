// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"
#include "private/Archiver.h"
#include "private/Core.h"
#include "private/StartupProjectSelectorDialog.h"

#include <Application.h>
#include <ProjectMetaAction.h>
#include <ManiVaultVersion.h>

#include <util/Icon.h>

#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QQuickWindow>
#include <QCommandLineParser>
#include <QTemporaryDir>
#include <QFileInfo>

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

QSharedPointer<ProjectMetaAction> getStartupProjectMetaAction(const QString& startupProjectFilePath)
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

        return QSharedPointer<ProjectMetaAction>(new ProjectMetaAction(extractedMetaJsonFilePath));
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

    QString startupProjectFilePath;
    QSharedPointer<ProjectMetaAction> startupProjectMetaAction;

    if (commandLineParser.isSet("project")) {
        QVector<QPair<QSharedPointer<mv::ProjectMetaAction>, QString>> startupProjectsMetaActionsCandidates;

        auto startupProjectsFilePathsCandidates = commandLineParser.value("project").split(",");

        for (const auto& startupProjectFilePathCandidate : QSet(startupProjectsFilePathsCandidates.begin(), startupProjectsFilePathsCandidates.end())) {
            if (!QFileInfo(startupProjectFilePathCandidate).exists())
                continue;

            auto startupProjectMetaActionCandidate = getStartupProjectMetaAction(startupProjectFilePathCandidate);

            if (startupProjectMetaActionCandidate.isNull())
                continue;

            startupProjectsMetaActionsCandidates << QPair<QSharedPointer<mv::ProjectMetaAction>, QString>(startupProjectMetaActionCandidate, startupProjectFilePathCandidate);
        }

        if (startupProjectsMetaActionsCandidates.count() >= 2) {
            StartupProjectSelectorDialog startupProjectSelectorDialog(startupProjectsMetaActionsCandidates);

            const auto dialogResult = startupProjectSelectorDialog.exec();

            if (dialogResult == QDialog::Accepted) {
                const auto selectedStartupProjectIndex = startupProjectSelectorDialog.getSelectedStartupProjectIndex();

                if (selectedStartupProjectIndex >= 0) {
                    startupProjectMetaAction    = startupProjectsMetaActionsCandidates[selectedStartupProjectIndex].first;
                    startupProjectFilePath      = startupProjectsMetaActionsCandidates[selectedStartupProjectIndex].second;
                }
            }

            if (dialogResult == QDialog::Rejected)
                return 0;

        } else {
            if (startupProjectsMetaActionsCandidates.count() == 1) {
                startupProjectMetaAction    = startupProjectsMetaActionsCandidates.first().first;
                startupProjectFilePath      = startupProjectsMetaActionsCandidates.first().second;
            }
        }
    }

    Application::setWindowIcon(createIcon(QPixmap(":/Icons/AppIcon256")));

    Core core;

    application.setCore(&core);

    core.createManagers();

    SplashScreenAction splashScreenAction(&application, false);

    if (!startupProjectMetaAction.isNull()) {
        try {
            const auto startupProjectFileInfo = QFileInfo(startupProjectFilePath);

            if (startupProjectFileInfo.exists()) {
                qDebug() << "Loading startup project from" << startupProjectFilePath;

                //ModalTask::getGlobalHandler()->setEnabled(false);

                application.setStartupProjectFilePath(startupProjectFilePath);
                
                application.getStartupTask().getLoadProjectTask().setEnabled(true, true);

                splashScreenAction.setProjectMetaAction(startupProjectMetaAction.get());
                splashScreenAction.fromVariantMap(startupProjectMetaAction->getSplashScreenAction().toVariantMap());

                application.setStartupProjectMetaAction(startupProjectMetaAction.get());

                if (startupProjectMetaAction->getReadOnlyAction().isChecked() && startupProjectMetaAction->getApplicationIconAction().getOverrideAction().isChecked())
                    application.setWindowIcon(startupProjectMetaAction->getApplicationIconAction().getIconPickerAction().getIcon());
            }
            else {
                splashScreenAction.addAlert(SplashScreenAction::Alert::warning(QString("\
                    Unable to load <b>%1</b> at startup, the file does not exist. \
                    Provide an existing project file path when using <b>-p</b>/<b>--project</b> ManiVault<sup>&copy;</sup> command line parameters. \
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

#ifdef _WIN32
    application.setStyle(new NoFocusProxyStyle);
#endif
    
#ifdef __APPLE__
    application.setStyle(QStyleFactory::create("Fusion"));
#endif

#ifdef _WIN32
    //QFile styleSheetFile(":/styles/default.qss");

    //styleSheetFile.open(QFile::ReadOnly);

    //QString styleSheet = QLatin1String(styleSheetFile.readAll());

    //application.setStyleSheet(styleSheet);
#endif
    
    loadGuiTask.setSubtaskFinished("Apply styles");

    MainWindow mainWindow;

    loadGuiTask.setSubtaskStarted("Create main window");

    mainWindow.show();

    loadGuiTask.setSubtaskFinished("Create main window");

    return application.exec();
}
