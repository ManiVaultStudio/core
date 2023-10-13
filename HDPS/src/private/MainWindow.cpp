// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MainWindow.h"
#include "PluginManager.h"
#include "StartPageWidget.h"
#include "ProjectWidget.h"
#include "FileMenu.h"
#include "ViewMenu.h"
#include "HelpMenu.h"

#include <Application.h>
#include <CoreInterface.h>

#include <ForegroundTask.h>
#include <ForegroundTaskHandler.h>
#include <BackgroundTask.h>
#include <BackgroundTaskHandler.h>

#include <actions/ToggleAction.h>

#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QMenuBar>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QMessageBox>
#include <QStackedWidget>
#include <QStatusBar>

#ifdef _DEBUG
    #define MAIN_WINDOW_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

/** 
 * Custom StackedWidget class to store the start page and project page widgets
 * 
 * QStackedWidget, by default, returns the largest minimum size of all widgets
 * We are interested in the size of the currently shown widget instead
 */
class StackedWidget : public QStackedWidget
{
    QSize sizeHint() const override
    {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override
    {
        return currentWidget()->minimumSizeHint();
    }
};

MainWindow::MainWindow(QWidget* parent /*= nullptr*/) :
    QMainWindow(parent)
{
    // Delay execution till the event loop has started, otherwise we cannot quit the application
    QTimer::singleShot(1000, this, &MainWindow::checkGraphicsCapabilities);

    restoreWindowGeometryFromSettings();
}

void MainWindow::showEvent(QShowEvent* showEvent)
{
    auto& loadGuiTask = Application::current()->getStartupTask().getLoadGuiTask();

    auto fileMenuAction = menuBar()->addMenu(new FileMenu());
    auto viewMenuAction = menuBar()->addMenu(new ViewMenu());
    auto helpMenuAction = menuBar()->addMenu(new HelpMenu());

    loadGuiTask.setSubtaskStarted("Initializing start page");

    auto stackedWidget      = new StackedWidget();
    auto projectWidget      = new ProjectWidget();
    auto startPageWidget    = new StartPageWidget(projectWidget);

    stackedWidget->addWidget(startPageWidget);
    stackedWidget->addWidget(projectWidget);

    setCentralWidget(stackedWidget);

    loadGuiTask.setSubtaskFinished("Initializing start page");

    statusBar()->setSizeGripEnabled(false);

    statusBar()->insertPermanentWidget(0, new QWidget(this), 3);
    statusBar()->insertPermanentWidget(1, BackgroundTask::getGlobalHandler()->getStatusBarAction()->createWidget(this), 2);
    statusBar()->insertPermanentWidget(2, ForegroundTask::getGlobalHandler()->getStatusBarAction()->createWidget(this));

    const auto projectChanged = [this]() -> void {
        if (!projects().hasProject()) {
            setWindowTitle("ManiVault");
        }
        else {
            const auto projectFilePath = projects().getCurrentProject()->getFilePath();

            if (projectFilePath.isEmpty())
                setWindowTitle("Unsaved - ManiVault");
            else
                setWindowTitle(QString("%1 - ManiVault").arg(projectFilePath));
        }

        statusBar()->setVisible(projects().hasProject());
    };

    connect(&projects(), &AbstractProjectManager::projectCreated, this, projectChanged);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, projectChanged);
    connect(&projects(), &AbstractProjectManager::projectOpened, this, projectChanged);
    connect(&projects(), &AbstractProjectManager::projectSaved, this, projectChanged);

    const auto toggleStartPage = [this, stackedWidget, projectWidget, startPageWidget]() -> void {
        if (projects().getShowStartPageAction().isChecked())
            stackedWidget->setCurrentWidget(startPageWidget);
        else
            stackedWidget->setCurrentWidget(projectWidget);
    };

    connect(&projects().getShowStartPageAction(), &ToggleAction::toggled, this, toggleStartPage);

    const auto updateMenuVisibility = [fileMenuAction, viewMenuAction]() -> void {
        const auto projectIsReadOnly = projects().getCurrentProject()->getReadOnlyAction().isChecked();

        fileMenuAction->setVisible(!projectIsReadOnly);
        viewMenuAction->setVisible(!projectIsReadOnly);
    };

    connect(&projects(), &AbstractProjectManager::projectCreated, this, [this, updateMenuVisibility]() -> void {
        connect(&projects().getCurrentProject()->getReadOnlyAction(), &ToggleAction::toggled, this, updateMenuVisibility);
    });

    loadGuiTask.setFinished();

    if (Application::current()->shouldOpenProjectAtStartup())
        projects().openProject(Application::current()->getStartupProjectFilePath());

    projectChanged();

    emit Application::current()->mainWindowInitialized();
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
    
    connect(qApp, &QApplication::paletteChanged, this, &MainWindow::updateStyle);
}

void MainWindow::updateStyle()
{
    this->style()->unpolish(this);
    this->style()->polish(this);

    QList<QWidget*> Children = this->findChildren<QWidget*>(QString(), Qt::FindChildrenRecursively);
    for (auto Widget : Children)
    {
        Widget->style()->unpolish(Widget);
        Widget->style()->polish(Widget);
    }
}
