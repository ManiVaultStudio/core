#include "MainWindow.h"
#include "PluginManager.h"
#include "StartPageWidget.h"
#include "ProjectWidget.h"
#include "FileMenu.h"
#include "ViewMenu.h"
#include "HelpMenu.h"

#include <Application.h>

#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QMenuBar>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QMessageBox>
#include <QTimer>
#include <QStackedWidget>

#define MAIN_WINDOW_VERBOSE

using namespace hdps;

MainWindow::MainWindow(QWidget* parent /*= nullptr*/) :
    QMainWindow(parent),
    _core()
{
    dynamic_cast<Application*>(qApp)->setCore(&_core);

    // Delay execution till the event loop has started, otherwise we cannot quit the application
    QTimer::singleShot(1000, this, &MainWindow::checkGraphicsCapabilities);

    restoreWindowGeometryFromSettings();
}

void MainWindow::showEvent(QShowEvent* showEvent)
{
    QMainWindow::showEvent(showEvent);

    if (!_core.isInitialized()) {
        _core.init();

        menuBar()->addMenu(new FileMenu());
        menuBar()->addMenu(new ViewMenu());
        menuBar()->addMenu(new HelpMenu());

        auto stackedWidget      = new QStackedWidget();
        auto projectWidget      = new ProjectWidget();
        auto startPageWidget    = new StartPageWidget(projectWidget);

        stackedWidget->addWidget(startPageWidget);
        stackedWidget->addWidget(projectWidget);

        setCentralWidget(stackedWidget);

        const auto updateWindowTitle = [&]() -> void {
            if (!projects().hasProject()) {
                setWindowTitle("HDPS");
            }
            else {
                const auto projectFilePath = projects().getCurrentProject()->getFilePath();

                if (projectFilePath.isEmpty())
                    setWindowTitle("Unsaved - HDPS");
                else
                    setWindowTitle(QString("%1 - HDPS").arg(projectFilePath));
            }
        };

        connect(&projects(), &ProjectManager::projectCreated, this, updateWindowTitle);
        connect(&projects(), &ProjectManager::projectDestroyed, this, updateWindowTitle);
        connect(&projects(), &ProjectManager::projectOpened, this, updateWindowTitle);
        connect(&projects(), &ProjectManager::projectSaved, this, updateWindowTitle);

        const auto toggleStartPage = [this, stackedWidget, projectWidget, startPageWidget]() -> void {
            if (projects().getShowStartPageAction().isChecked())
                stackedWidget->setCurrentWidget(startPageWidget);
            else
                stackedWidget->setCurrentWidget(projectWidget);
        };

        connect(&projects().getShowStartPageAction(), &ToggleAction::toggled, this, toggleStartPage);

        if (Application::current()->shouldOpenProjectAtStartup())
            projects().openProject(Application::current()->getStartupProjectFilePath());

        connect(&projects(), &ProjectManager::projectOpened, this, [this](const Project& project) -> void {
            menuBar()->setVisible(project.getReadOnlyAction().isChecked());
        });
    }
}

void MainWindow::closeEvent(QCloseEvent* closeEvent)
{
}

void MainWindow::moveEvent(QMoveEvent* moveEvent)
{
    saveWindowGeometryToSettings();

    QWidget::moveEvent(moveEvent);
}

void MainWindow::resizeEvent(QResizeEvent* resizeEvent)
{
    saveWindowGeometryToSettings();

    QWidget::resizeEvent(resizeEvent);
}

void MainWindow::restoreWindowGeometryFromSettings()
{
    const auto storedMainWindowGeometry = Application::current()->getSetting("MainWindow/Geometry", QVariant());

    QRect mainWindowRect;

    if (storedMainWindowGeometry.isValid())
        restoreGeometry(storedMainWindowGeometry.toByteArray());
    else
        setDefaultWindowGeometry();
}

void MainWindow::saveWindowGeometryToSettings()
{
    Application::current()->setSetting("MainWindow/Geometry", saveGeometry());
}

void MainWindow::setDefaultWindowGeometry(const float& coverage /*= 0.7f*/) {
    const auto primaryScreen        = qApp->primaryScreen();
    const auto availableGeometry    = primaryScreen->availableGeometry();
    const auto availableSize        = availableGeometry.size();
    const auto newSize              = QSize(availableSize.width() * coverage, availableSize.height() * coverage);
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newSize, availableGeometry));
}

void MainWindow::checkGraphicsCapabilities()
{
#ifdef MAIN_WINDOW_VERBOSE
    qDebug() << "Checking graphics card capabilities";
#endif

    QOffscreenSurface surf;
    surf.create();

    QOpenGLContext ctx;
    ctx.create();
    ctx.makeCurrent(&surf);

    QOpenGLFunctions gl;
    gl.initializeOpenGLFunctions();

    int majorVersion, minorVersion;
    gl.glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
    gl.glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

    QString warningMessage = "HDPS requires a graphics card with support for OpenGL 3.3 or newer. "
                             "The currently used card supports version " + QString::number(majorVersion) + 
                             "." + QString::number(minorVersion) + ". Make sure the application uses your "
                             "dedicated graphics card and update your drivers. "
                             "Please visit https://github.com/hdps/PublicWiki/wiki/Graphics-card-issues to "
                             "learn more about this issue.";

    if (majorVersion < 3 || (majorVersion == 3 && minorVersion < 3))
    {
        int ret = QMessageBox::warning(this, tr("HDPS"),
            tr(warningMessage.toStdString().c_str()));

        QApplication::exit(33);
    }

    ctx.doneCurrent();
}
