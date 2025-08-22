// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "MainWindow.h"
#include "PluginManager.h"
#include "StartPageWidget.h"
#include "LearningPageWidget.h"
#include "ProjectWidget.h"
#include "FileMenu.h"
#include "ViewMenu.h"
#include "ProjectsMenu.h"
#include "HelpMenu.h"

#include <Application.h>
#include <CoreInterface.h>

#include <BackgroundTaskHandler.h>

#include <actions/ToggleAction.h>
#include <actions/PluginStatusBarAction.h>

#include "FrontPagesStatusBarAction.h"
#include "ManiVaultVersionStatusBarAction.h"
#include "PluginsStatusBarAction.h"
#include "LoggingStatusBarAction.h"
#include "BackgroundTasksStatusBarAction.h"
#include "SettingsStatusBarAction.h"
#include "WorkspaceStatusBarAction.h"

#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QMenuBar>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QStackedWidget>
#include <QStatusBar>
#include <QRandomGenerator>

#ifdef _DEBUG
    #define MAIN_WINDOW_VERBOSE
#endif

//#define MAIN_WINDOW_SCREEN_CAPTURE

using namespace mv;
using namespace mv::gui;

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
    QMainWindow::showEvent(showEvent);

    if (mv::workspaces().hasWarmedUpNativeWidgets() && centralWidget() == nullptr) {
        auto& loadGuiTask = Application::current()->getStartupTask().getLoadGuiTask();

        auto fileMenuAction     = menuBar()->addMenu(new FileMenu());
        auto viewMenuAction     = menuBar()->addMenu(new ViewMenu());
        auto projectsMenuAction = menuBar()->addMenu(new ProjectsMenu());
        auto helpMenuAction     = menuBar()->addMenu(new HelpMenu());

        loadGuiTask.setSubtaskStarted("Initializing start page");

        auto stackedWidget      = new StackedWidget();
        auto projectWidget      = new ProjectWidget();
        auto startPageWidget    = new StartPageWidget(projectWidget);
        auto learningPageWidget = new LearningPageWidget();

        stackedWidget->addWidget(startPageWidget);
        stackedWidget->addWidget(projectWidget);
        stackedWidget->addWidget(learningPageWidget);

        setCentralWidget(stackedWidget);

        loadGuiTask.setSubtaskFinished("Initializing start page");

        statusBar()->setSizeGripEnabled(false);

        auto startPageStatusBarAction       = new FrontPagesStatusBarAction(this, "Start Page");
        auto versionStatusBarAction         = new ManiVaultVersionStatusBarAction(this, "Version");
        auto pluginsStatusBarAction         = new PluginsStatusBarAction(this, "Plugins");
        auto loggingStatusBarAction         = new LoggingStatusBarAction(this, "Logging");
        auto backgroundTasksStatusBarAction = new BackgroundTasksStatusBarAction(this, "Background Tasks");
        auto settingsTasksStatusBarAction   = new SettingsStatusBarAction(this, "Settings");
        auto workspaceStatusBarAction       = new WorkspaceStatusBarAction(this, "Workspace");

        statusBar()->insertPermanentWidget(0, startPageStatusBarAction->createWidget(this));
        statusBar()->insertPermanentWidget(1, versionStatusBarAction->createWidget(this));
        statusBar()->insertPermanentWidget(2, pluginsStatusBarAction->createWidget(this));
        statusBar()->insertPermanentWidget(3, workspaceStatusBarAction->createWidget(this));
        statusBar()->insertPermanentWidget(4, loggingStatusBarAction->createWidget(this), 4);
        statusBar()->insertPermanentWidget(5, backgroundTasksStatusBarAction->createWidget(this), 1);
        statusBar()->insertPermanentWidget(6, settingsTasksStatusBarAction->createWidget(this));

        const auto updateStatusBarVisibility = [this]() -> void {
            statusBar()->setVisible(mv::projects().hasProject() && mv::settings().getMiscellaneousSettings().getStatusBarVisibleAction().isChecked());
        };

        const auto getNumberOfPermanentWidgets = [this]() -> std::int32_t {
            return statusBar()->findChildren<QWidget*>(Qt::FindDirectChildrenOnly).count();
        };

        for (auto pluginFactory : mv::plugins().getPluginFactoriesByTypes()) {
            if (auto statusBarAction = pluginFactory->getStatusBarAction()) {
                const auto index = statusBarAction->getIndex();

                if (index == 0)
                    statusBar()->addPermanentWidget(statusBarAction->createWidget(this), statusBarAction->getStretch());

                if (index <= -1)
                    statusBar()->insertPermanentWidget(std::max(0, getNumberOfPermanentWidgets() + index), statusBarAction->createWidget(this), statusBarAction->getStretch());

                if (index >= 1)
                    statusBar()->insertPermanentWidget(index - 1, statusBarAction->createWidget(this), statusBarAction->getStretch());
            }
        }

        const auto projectChanged = [this, updateStatusBarVisibility]() -> void {
            if (!projects().hasProject()) {
                setWindowTitle("ManiVault");
            }
            else {
                if (projects().getCurrentProject()->getReadOnlyAction().isChecked()) {
                    setWindowTitle(projects().getCurrentProject()->getTitleAction().getString());
                } else {
                    const auto projectFilePath = projects().getCurrentProject()->getFilePath();

                    if (projectFilePath.isEmpty())
                        setWindowTitle("Unsaved - ManiVault");
                    else
                        setWindowTitle(QString("%1 - ManiVault").arg(projectFilePath));
                }
            }

            updateStatusBarVisibility();
        };

        connect(&projects(), &AbstractProjectManager::projectCreated, this, projectChanged);
        connect(&projects(), &AbstractProjectManager::projectDestroyed, this, projectChanged);
        connect(&projects(), &AbstractProjectManager::projectOpened, this, projectChanged);
        connect(&projects(), &AbstractProjectManager::projectSaved, this, projectChanged);

        const auto toggleStartPage = [this, stackedWidget, projectWidget, startPageWidget](bool toggled) -> void {
            if (toggled)
                stackedWidget->setCurrentWidget(startPageWidget);
            else
                stackedWidget->setCurrentWidget(projectWidget);
        };

        connect(&projects().getShowStartPageAction(), &ToggleAction::toggled, this, toggleStartPage);

        connect(&help().getShowLearningCenterPageAction(), &ToggleAction::toggled, this, [stackedWidget, startPageWidget, projectWidget, learningPageWidget](bool toggled) -> void {
            if (toggled)
                stackedWidget->setCurrentWidget(learningPageWidget);
            else {
                if (projects().hasProject())
                    stackedWidget->setCurrentWidget(projectWidget);
                else
                    stackedWidget->setCurrentWidget(startPageWidget);
            }
        });

        const auto updateMenuVisibility = [fileMenuAction, projectsMenuAction]() -> void {
            const auto projectIsReadOnly = projects().getCurrentProject()->getReadOnlyAction().isChecked();

            fileMenuAction->setVisible(!projectIsReadOnly);
            projectsMenuAction->setVisible(projectIsReadOnly ? projects().getCurrentProject()->getAllowProjectSwitchingAction().isChecked() : true);
        };

        connect(&projects(), &AbstractProjectManager::projectCreated, this, [this, updateMenuVisibility]() -> void {
            connect(&projects().getCurrentProject()->getReadOnlyAction(), &ToggleAction::toggled, this, updateMenuVisibility);
        });

        loadGuiTask.setFinished();

        if (Application::current()->shouldOpenProjectAtStartup())
            projects().openProject(Application::current()->getStartupProjectUrl());

        projectChanged();
        updateStatusBarVisibility();

        connect(&mv::settings().getMiscellaneousSettings().getStatusBarVisibleAction(), &ToggleAction::toggled, this, updateStatusBarVisibility);

        mv::help().initializeNotifications(this);
    }
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

    if (storedMainWindowGeometry.isValid())
        restoreGeometry(storedMainWindowGeometry.toByteArray());
    else
        setDefaultWindowGeometry();

#ifdef MAIN_WINDOW_SCREEN_CAPTURE
    setFixedSize(1280, 720);
#endif
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

    QString warningMessage = "ManiVault requires a graphics card with support for OpenGL 3.3 or newer. "
                             "The currently used card supports version " + QString::number(majorVersion) + 
                             "." + QString::number(minorVersion) + ". Make sure the application uses your "
                             "dedicated graphics card and update your drivers. "
                             "Please visit https://github.com/ManiVaultStudio/PublicWiki/wiki/FAQ#manivault-launches-with-a-warning-my-graphics-card-does-not-support-opengl-33 "
                             "to learn more about this issue.";

    if (majorVersion < 3 || (majorVersion == 3 && minorVersion < 3))
    {
        int ret = QMessageBox::warning(this, tr("ManiVault"),
            tr(warningMessage.toStdString().c_str()));

        QApplication::exit(33);
    }

    ctx.doneCurrent();
    
}
