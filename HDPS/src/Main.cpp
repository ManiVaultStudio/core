#include "MainWindow.h"

#include <QSurfaceFormat>
#include <QStyleFactory>

#include <HdpsApplication.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("BioVault");
    QCoreApplication::setOrganizationDomain("LUMC (LKEB) & TU Delft (CGV)");
    QCoreApplication::setApplicationName("HDPS");
    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

#ifdef __APPLE__
    // Ask for an OpenGL 3.3 Core Context as the default
    QSurfaceFormat defaultFormat;
    defaultFormat.setVersion(3, 3);
    defaultFormat.setProfile(QSurfaceFormat::CoreProfile);
    defaultFormat.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    QSurfaceFormat::setDefaultFormat(defaultFormat);
#endif

    hdps::HdpsApplication hdpsApplication(argc, argv);

    // Retina display support for Mac OS and X11:
    // AA_UseHighDpiPixmaps attribute is off by default in Qt 5.1 but will most
    // likely be on by default in a future release of Qt.
    hdpsApplication.setAttribute(Qt::AA_UseHighDpiPixmaps);
    
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

    hdpsApplication.setWindowIcon(appIcon);

    hdps::gui::MainWindow mainWindow;

    mainWindow.show();

    return hdpsApplication.exec();
}
