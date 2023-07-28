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

/**
 * Tasks popup widget class
 *
 * Popup widget class for showing tasks, the visibility depends on whether there are tasks running.
 *
 * @author Thomas Kroes
 */
class TasksPopupWidget : public QWidget
{
public:

    /**
     * Construct with \p anchorWidget
     * @param anchorWidget Pointer to anchor widget
     */
    TasksPopupWidget(QWidget* anchorWidget) :
        QWidget(),
        _anchorWidget(anchorWidget),
        _tasksAction(this, "Tasks")
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);// | Qt::WindowStaysOnTopHint);// Qt::Window);// Qt::Popup);// Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setFixedSize(QSize(400, 200));
        setObjectName("TasksPopupWidget");
        setStyleSheet("QWidget#TasksPopupWidget { border: 5p solid rgb(74, 74, 74); }");
        hide();

        auto& tasksFilterModel = _tasksAction.getTasksFilterModel();

        tasksFilterModel.getStatusFilterAction().setSelectedOptions({ "Idle", "Running", "Running Indeterminate" });

        auto layout = new QVBoxLayout();

        layout->setContentsMargins(4, 4, 4, 4);
        layout->addWidget(_tasksAction.createWidget(this));

        setLayout(layout);

        installEventFilter(this);

        _anchorWidget->installEventFilter(this);

        const auto updateVisibility = [this]() -> void {
            const auto numberOfRows = _tasksAction.getTasksFilterModel().rowCount();

            if (numberOfRows == 0 && isVisible())
                QTimer::singleShot(250, Qt::PreciseTimer, this, [this]() -> void { hide(); });

            if (numberOfRows >= 1 && !isVisible())
                show();
        };

        updateVisibility();

        auto& taskFilterModel = _tasksAction.getTasksFilterModel();

        connect(&taskFilterModel, &QSortFilterProxyModel::rowsInserted, updateVisibility);
        connect(&taskFilterModel, &QSortFilterProxyModel::rowsRemoved, updateVisibility);
        connect(&taskFilterModel, &QSortFilterProxyModel::layoutChanged, updateVisibility);
    }

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override {
        switch (event->type())
        {
            case QEvent::Resize:
            case QEvent::Move:
            {
                if (target == this)
                    break;

                updatePosition();
                break;
            }

            default:
                break;
        }

        return QWidget::eventFilter(target, event);
    }

    /**
     * Override popup minimum size hint
     * @return Minimum size hint
     */
    QSize minimumSizeHint() const override {
        return QSize(800, 400);
    }

    /**
     * Override popup size hint
     * @return Size hint
     */
    QSize sizeHint() const override {
        return minimumSizeHint();
    }

private:

    /** Updates the position relative to the parent widget */
    void updatePosition() {
        const auto globalAnchorWidgetTopRight = _anchorWidget->mapToGlobal(_anchorWidget->rect().topRight());
        
        setGeometry(QRect(globalAnchorWidgetTopRight - QPoint(width(), height()), size()));
    }

private:
    QWidget*        _anchorWidget;  /** Pointer to widget from which the popup originates */
    TasksAction     _tasksAction;   /** Action which shows all tasks */
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

        if (Application::current()->shouldOpenProjectAtStartup())
            projects().openProject(Application::current()->getStartupProjectFilePath());
    
        updateWindowTitle();

        auto tasksLabel         = new QPushButton();
        auto tasksPopupWidget   = new TasksPopupWidget(tasksLabel);

        tasksLabel->setEnabled(false);
        tasksLabel->setFlat(true);
        tasksLabel->setIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));

        statusBar()->setSizeGripEnabled(false);
        statusBar()->insertPermanentWidget(0, tasksLabel);
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
