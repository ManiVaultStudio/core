// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "private/MainWindow.h"

#include <Application.h>

#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QProxyStyle>
#include <QQuickWindow>
#include <QCommandLineParser>

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

    hdps::Application application(argc, argv);

    QCommandLineParser commandLineParser;

    commandLineParser.setApplicationDescription("Application for viewing and analyzing high-dimensional data");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();

    QCommandLineOption projectOption({ "p","project" }, "File path of the project to load upon startup", "project");

    commandLineParser.addOption(projectOption);
    commandLineParser.process(QCoreApplication::arguments());
    
    if (commandLineParser.isSet("project"))
        application.setStartupProjectFilePath(commandLineParser.value("project"));

    application.setStyle(new NoFocusProxyStyle);

    QFile styleSheetFile(":/styles/default.qss");

    styleSheetFile.open(QFile::ReadOnly);

    QString styleSheet = QLatin1String(styleSheetFile.readAll());

    qApp->setStyleSheet(styleSheet);

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
