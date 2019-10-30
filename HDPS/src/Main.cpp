#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("CGV TU Delft");
    QCoreApplication::setOrganizationDomain("graphics.tudelft.nl");
    QCoreApplication::setApplicationName("HDPS");
    // Necessary to instantiate QWebEngine from a plugin
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);

    QApplication app(argc, argv);

    // Retina display support for Mac OS and X11:
    // AA_UseHighDpiPixmaps attribute is off by default in Qt 5.1 but will most
    // likely be on by default in a future release of Qt.
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);

    QIcon appIcon;
    appIcon.addFile(":/Icons/AppIcon32");
    appIcon.addFile(":/Icons/AppIcon64");
    appIcon.addFile(":/Icons/AppIcon128");
    appIcon.addFile(":/Icons/AppIcon256");
    appIcon.addFile(":/Icons/AppIcon512");
    appIcon.addFile(":/Icons/AppIcon1024");
    app.setWindowIcon(appIcon);

    hdps::gui::MainWindow mainWindow;
    mainWindow.centerAndResize(0.75f);
    mainWindow.show();

    return app.exec();
}
