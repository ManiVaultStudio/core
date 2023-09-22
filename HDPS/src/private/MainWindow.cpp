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
#include "Task.h"

#include <Application.h>

#include <widgets/HierarchyWidget.h>

#include <actions/TasksAction.h>

#include <TasksModel.h>

#include <ForegroundTask.h>
#include <ForegroundTaskHandler.h>

#include <QDebug>
#include <QMessageBox>
#include <QScreen>
#include <QMenuBar>
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QMessageBox>
#include <QTimer>
#include <QStackedWidget>
#include <QStatusBar>
#include <QGroupBox>
#include <QToolButton>
#include <QPainter>

#define MAIN_WINDOW_VERBOSE

using namespace hdps;

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

class StatusBarToolButton : public QToolButton
{
protected:
    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed in a popup
     */
    StatusBarToolButton(QWidget* parent, WidgetAction* action) :
        QToolButton(parent),
        _action(action)
    {
        Q_ASSERT(_action != nullptr);

        setPopupMode(QToolButton::InstantPopup);
        setStyleSheet("QToolButton::menu-indicator { image: none; }");
        setAutoRaise(true);
        setIconSize(QSize(18, 18));

        addAction(_action);

        const auto updateToolButton = [this]() {
            setEnabled(_action->isEnabled());
            setToolTip(_action->toolTip());
            setVisible(_action->isVisible());

            update();
        };

        updateToolButton();

        connect(_action, &WidgetAction::changed, this, updateToolButton);

        /*
        auto popupWidget = new QWidget(this);

        //popupWidget->setWindowFlag(Qt::Popup);
        //popupWidget->setWindowFlag(Qt::Window);

        auto popupLayout = new QVBoxLayout();

        popupLayout->addWidget(new QPushButton("Test"));
        popupWidget->setLayout(popupLayout);
        popupWidget->raise();
        popupWidget->show();
        */
    }

public:

    /**
     * Paint event
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent) {
        QToolButton::paintEvent(paintEvent);

        QPainter painter(this);

        const auto margin   = 3;
        const auto icon     = _action->icon();

        if (icon.isNull())
            return;

        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawPixmap(QPoint(margin, margin), icon.pixmap(icon.availableSizes().first()).scaled(size() - 2 * QSize(margin, margin), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

private:
    WidgetAction* _action;

    friend class MainWindow;
};

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

        auto loadGuiTask = Application::current()->getTask(Application::TaskType::LoadGUI);

        loadGuiTask->setSubtaskStarted("Initializing GUI");

        auto fileMenuAction = menuBar()->addMenu(new FileMenu());
        auto viewMenuAction = menuBar()->addMenu(new ViewMenu());
        auto helpMenuAction = menuBar()->addMenu(new HelpMenu());

        auto stackedWidget      = new StackedWidget();
        auto projectWidget      = new ProjectWidget();
        auto startPageWidget    = new StartPageWidget(projectWidget);

        stackedWidget->addWidget(startPageWidget);
        stackedWidget->addWidget(projectWidget);

        setCentralWidget(stackedWidget);

        const auto updateWindowTitle = [&]() -> void {
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

        const auto updateMenuVisibility = [fileMenuAction, viewMenuAction]() -> void {
            const auto projectIsReadOnly = projects().getCurrentProject()->getReadOnlyAction().isChecked();

            fileMenuAction->setVisible(!projectIsReadOnly);
            viewMenuAction->setVisible(!projectIsReadOnly);
        };

        connect(&projects(), &ProjectManager::projectCreated, this, [this, updateMenuVisibility]() -> void {
            connect(&projects().getCurrentProject()->getReadOnlyAction(), &ToggleAction::toggled, this, updateMenuVisibility);
        });

        auto tasksAction = new TasksAction(this, "Tasks");
        auto overallBackgroundTaskAction = new TaskAction(this, "Background Tasks");

        tasksAction->setPopupSizeHint(QSize(500, 500));
        tasksAction->setDefaultWidgetFlags(TasksAction::Toolbar | TasksAction::Overlay | WidgetActionWidget::PopupLayout);

        overallBackgroundTaskAction->setTask(Application::current()->getTask(Application::TaskType::OverallBackground));

        statusBar()->setSizeGripEnabled(true);
        statusBar()->insertPermanentWidget(0, overallBackgroundTaskAction->createWidget(this));
        statusBar()->insertPermanentWidget(1, &ForegroundTask::getHandler()->getStatusBarButton());

        loadGuiTask->setSubtaskFinished("Initializing GUI");

        if (Application::current()->shouldOpenProjectAtStartup())
            projects().openProject(Application::current()->getStartupProjectFilePath());
    
        updateWindowTitle();

        emit Application::current()->mainWindowInitialized();
        
        loadGuiTask->setFinished("ManiVault Loaded", true, 1000);
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
