#include "MainWindow.h"
#include "ActionsViewerWidget.h"
#include "DataPropertiesWidget.h"
#include "DataManager.h"
#include "Logger.h"
#include "StartPageWidget.h"

#include "PluginManager.h"
#include "PluginType.h"
#include "Application.h"
#include "ViewPlugin.h"
#include "AnalysisPlugin.h"

#include "util/FileUtil.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QScreen>
#include <QToolBar>
#include <QDebug>

#include "DockWidgetTab.h"
#include "DockAreaTitleBar.h"
#include "DockSplitter.h"

// Graphics capability checking
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QMessageBox>
#include <QTimer>
#include <QLabel>

#define MAIN_WINDOW_VERBOSE

using namespace ads;

namespace hdps
{

namespace gui
{

MainWindow::MainWindow(QWidget *parent /*= nullptr*/) :
    QMainWindow(parent),
    _startPageWidget(nullptr),
    _dockManager(new CDockManager(this)),
    _centralDockArea(nullptr),
    _lastDockAreaWidget(nullptr),
    _loggingDockArea(nullptr),
    _centralDockWidget(new CDockWidget("Views")),
    _startPageDockWidget(new CDockWidget("Start page")),
    _loggingDockWidget(new CDockWidget("Logging")),
    _loadedViewPluginsMenu("Loaded views"),
    _pluginsHelpMenu("Plugins")
{
    setupUi(this);

    _core = QSharedPointer<Core>::create(*this);

    dynamic_cast<Application*>(qApp)->setCore(_core.get());

    _core->init();

    _startPageWidget        = new StartPageWidget(this);

    // Change the window title when the current project file changed
    connect(Application::current(), &Application::currentProjectFilePathChanged, [this](const QString& currentProjectFilePath) {
        setWindowTitle(currentProjectFilePath + (currentProjectFilePath.isEmpty() ? " HDPS" : " - HDPS"));
    });

    setupFileMenu();
    setupViewMenu();

    restoreWindowGeometryFromSettings();

    Logger::Initialize();

    // Delay execution till the event loop has started, otherwise we cannot quit the application
    QTimer::singleShot(1000, this, &MainWindow::checkGraphicsCapabilities);

    menuVisualization->addSection("Loaded");
    menuVisualization->addMenu(&_loadedViewPluginsMenu);

    _pluginsHelpMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));

    menuHelp->addSeparator();
    menuHelp->addMenu(&_pluginsHelpMenu);

    _loadedViewPluginsMenu.setEnabled(false);
}

void MainWindow::addImportOption(QAction* action)
{
    importDataMenu->addAction(action);
}

void MainWindow::addViewMenuAction(QAction* action)
{
    menuVisualization->insertAction(0, action);

    //if (menuVisualization->actions().count() == 2)
    //    menuVisualization->insertAction(1, &_loadedViewPluginsMenu);
}

void MainWindow::addLoadedViewPluginAction(QAction* action)
{
    _loadedViewPluginsMenu.addAction(action);

    _loadedViewPluginsMenu.setEnabled(true);
}

void MainWindow::addPluginTriggerHelpAction(QAction* action)
{
    _pluginsHelpMenu.addAction(action);
}

void MainWindow::showEvent(QShowEvent* showEvent)
{
    QMainWindow::showEvent(showEvent);

    initializeDocking();
}

void MainWindow::closeEvent(QCloseEvent* closeEvent)
{
    delete _dockManager;
}

void MainWindow::addPlugin(plugin::Plugin* plugin)
{
    const auto pluginType = plugin->getType();

    if (pluginType != plugin::Type::ANALYSIS && pluginType != plugin::Type::VIEW)
        return;

    auto dockWidget = new CDockWidget(plugin->getGuiName());

    dockWidget->setIcon(plugin->getIcon());

    switch (pluginType)
    {
        case plugin::Type::ANALYSIS:
        {
            auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

            dockWidget->setProperty("PluginType", "Analysis");

            break;
        }

        case plugin::Type::VIEW:
        {
            auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(plugin);

            dockWidget->setWidget(&viewPlugin->getWidget(), CDockWidget::ForceNoScrollArea);
            dockWidget->setProperty("PluginType", "View");

            connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, this, [this, dockWidget](const QString& title) {
                dockWidget->setWindowTitle(title);
            });

            connect(&viewPlugin->getMayCloseAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
                dockWidget->setFeature(CDockWidget::DockWidgetClosable, toggled);
            });

            connect(&viewPlugin->getMayFloatAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
                dockWidget->setFeature(CDockWidget::DockWidgetFloatable, toggled);
            });

            const auto connectToViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
                connect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, [this, dockWidget](bool toggled) {
                    dockWidget->toggleView(toggled);
                });
            };

            const auto disconnectFromViewPluginVisibleAction = [this, viewPlugin](CDockWidget* dockWidget) -> void {
                disconnect(&viewPlugin->getVisibleAction(), &ToggleAction::toggled, this, nullptr);
            };

            QObject::connect(dockWidget, &CDockWidget::closed, this, [this, viewPlugin, dockWidget, connectToViewPluginVisibleAction, disconnectFromViewPluginVisibleAction]() {
                disconnectFromViewPluginVisibleAction(dockWidget);
                {
                    viewPlugin->getVisibleAction().setChecked(false);
                }
                connectToViewPluginVisibleAction(dockWidget);
                
                updateCentralWidget();
            });

            connectToViewPluginVisibleAction(dockWidget);

            auto dockWidgetArea = RightDockWidgetArea;

            if (getViewPluginDockWidgets().isEmpty())
                dockWidgetArea = CenterDockWidgetArea;

            if (_lastDockAreaWidget == nullptr)
                _lastDockAreaWidget = _dockManager->addDockWidget(dockWidgetArea, dockWidget, _centralDockArea);
            else
                _lastDockAreaWidget = _dockManager->addDockWidget(dockWidgetArea, dockWidget, _lastDockAreaWidget);
            
            QObject::connect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, [this](int index) {
                updateCentralWidget();
            });
            
            

            QObject::connect(dockWidget, &CDockWidget::topLevelChanged, [this, dockWidget](bool topLevel) {
                updateCentralWidget();
            });
            
            updateCentralWidget();

            break;
        }

        default:
            break;
    }
}

void MainWindow::addDockWidget(QWidget* widget, const QString& windowTitle, const DockWidgetArea& dockWidgetArea, CDockAreaWidget* dockAreaWidget)
{
    auto dockWidget = new CDockWidget(windowTitle);

    _dockManager->addDockWidget(dockWidgetArea, dockWidget, dockAreaWidget);
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

void MainWindow::initializeDocking()
{
    CDockManager::setConfigFlag(CDockManager::DragPreviewIsDynamic, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, true);
    CDockManager::setConfigFlag(CDockManager::DragPreviewHasWindowFrame, true);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, false);
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
    CDockManager::setConfigFlag(CDockManager::EqualSplitOnInsertion, true);
    CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, true);

    initializeCentralDockingArea();
    initializeLoggingDockingArea();

    connect(_dockManager, &CDockManager::dockAreasAdded, this, &MainWindow::updateCentralWidget);
    connect(_dockManager, &CDockManager::dockAreasRemoved, this, &MainWindow::updateCentralWidget);
}

void MainWindow::initializeCentralDockingArea()
{
    _startPageDockWidget->setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
    _startPageDockWidget->setWidget(_startPageWidget, CDockWidget::ForceNoScrollArea);

    _centralDockArea = _dockManager->addDockWidget(DockWidgetArea::CenterDockWidgetArea, _startPageDockWidget);

    _startPageDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    _startPageDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    _startPageDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
}

void MainWindow::initializeLoggingDockingArea()
{
    _loggingDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("scroll"));

    _loggingDockArea = _dockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, _loggingDockWidget, _centralDockArea);

    _loggingDockArea->hide();
    _loggingDockArea->setMinimumHeight(100);
    _loggingDockArea->setMaximumHeight(300);
    _loggingDockArea->resize(QSize(0, 150));
}

void MainWindow::updateCentralWidget()
{
#ifdef MAIN_WINDOW_VERBOSE
    qDebug().noquote() << "Update central widget with" << getViewPluginDockWidgets().count() << "view plugins";
#endif

    if (getViewPluginDockWidgets().count() == 0) {

        // The only valid docking area is in the center when there is only one view plugin loaded
        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);

        _centralDockArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        // Show the start page dock widget and set the mode
        _startPageDockWidget->toggleView(true);
        _startPageWidget->setMode(_core->requestAllDataSets().size() == 0 ? StartPageWidget::Mode::ProjectBar : StartPageWidget::Mode::LogoOnly);
    }
    else {

        // Docking may occur anywhere
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

        // Hide the start page dock widget
        _startPageDockWidget->toggleView(false);
    }
}

QList<ads::CDockWidget*> MainWindow::getViewPluginDockWidgets(const bool& openOnly /*= true*/)
{
    QList<ads::CDockWidget*> viewPluginDockWidgets;

    for (auto viewPluginDockWidget : _dockManager->dockWidgetsMap().values()) {
        if (viewPluginDockWidget->property("PluginType").toString() != "View")
            continue;

        if (openOnly) {
            if (!viewPluginDockWidget->isClosed() && !viewPluginDockWidget->isFloating())
                viewPluginDockWidgets << viewPluginDockWidget;
        }
        else {
            viewPluginDockWidgets << viewPluginDockWidget;
        }
    }

    return viewPluginDockWidgets;
}

void MainWindow::setupFileMenu()
{
    // Set action icons
    openProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    saveProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    saveProjectAsAction->setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    recentProjectsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("clock"));
    clearDatasetsAction->setIcon(Application::getIconFont("FontAwesome").getIcon("trash"));
    importDataMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    exitAction->setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));

    // Set action tooltips
    openProjectAction->setToolTip("Open project from disk");
    saveProjectAction->setToolTip("Save project to disk");
    saveProjectAsAction->setToolTip("Save project to disk in a chosen location");
    recentProjectsMenu->setToolTip("Recently opened HDPS projects");
    clearDatasetsAction->setToolTip("Reset the data model");
    exitAction->setToolTip("Exit the HDPS application");

    // Load project when action is triggered
    connect(openProjectAction, &QAction::triggered, this, []() -> void {
        Application::current()->loadProject();
    });

    // Save project without user interaction when action is triggered
    connect(saveProjectAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject(Application::current()->getCurrentProjectFilePath());
    });

    // Save project to picked location when action is triggered
    connect(saveProjectAsAction, &QAction::triggered, [this](bool) {
        Application::current()->saveProject();
    });

    // Reset the data model when the action is triggered
    connect(clearDatasetsAction, &QAction::triggered, this, [this]() -> void {

        // Get all loaded datasets (irrespective of the data type)
        const auto loadedDatasets = _core->requestAllDataSets();

        // The project needs to be cleared if there are one or more datasets loaded
        if (!loadedDatasets.empty()) {

            // Check in the settings if the user has to be prompted with a question whether to automatically remove all datasets
            if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {

                // Ask for confirmation dialog
                DataRemoveAction::ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Remove all datasets", loadedDatasets);

                // Show the confirm data removal dialog
                confirmDataRemoveDialog.exec();

                // Remove dataset and children from the core if accepted
                if (confirmDataRemoveDialog.result() == 1)
                    Application::core()->removeAllDatasets();
            }
        }
    });

    // Close the main window then the exit action is triggered
    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QObject::connect(findLogFileAction, &QAction::triggered, [this](bool) {
        const auto filePath = Logger::GetFilePathName();

        if (!hdps::util::ShowFileInFolder(filePath))
        {
            QMessageBox::information(this,
                QObject::tr("Log file not found"),
                QObject::tr("The log file is not found:\n%1").arg(filePath));
        }
    });

    QObject::connect(logViewAction, &QAction::triggered, [this](const bool checked) {
        if (checked) {
            _loggingDockWidget->setWidget(new LogDockWidget(*this));
            _loggingDockArea->show();
        }
        else {
            delete _loggingDockWidget->takeWidget();
            _loggingDockArea->hide();
        }
    });

    // Update read-only status of various actions when the main file menu is opened
    connect(fileMenu, &QMenu::aboutToShow, this, [this]() -> void {

        // Establish whether there are any loaded datasets
        const auto hasDatasets = _core->requestAllDataSets().size();

        // Update read-only status
        saveProjectAction->setEnabled(!Application::current()->getCurrentProjectFilePath().isEmpty());
        saveProjectAsAction->setEnabled(hasDatasets);
        clearDatasetsAction->setEnabled(hasDatasets);

        // Populate the recent projects menu
        populateRecentProjectsMenu();
    });
}

void MainWindow::setupViewMenu()
{
    /*
    // Set action icons
    dataHierarchyViewerAction->setIcon(Application::getIconFont("FontAwesome").getIcon("sitemap"));
    dataPropertiesViewerAction->setIcon(Application::getIconFont("FontAwesome").getIcon("edit"));

    // Set action tooltips
    dataHierarchyViewerAction->setToolTip("Show/hide the data hierarchy viewer");
    dataPropertiesViewerAction->setToolTip("Show/hide the data properties viewer");

    // Show/hide data hierarchy dock widget when the corresponding action is toggled
    connect(dataHierarchyViewerAction, &QAction::toggled, this, [this](bool toggled) -> void {
        _dataHierarchyDockWidget->dockAreaWidget()->setVisible(toggled);
    });

    // Show/hide data properties dock widget when the corresponding action is toggled
    connect(dataPropertiesViewerAction, &QAction::toggled, this, [this](bool toggled) -> void {
        _dataPropertiesDockWidget->dockAreaWidget()->setVisible(toggled);
    });
    */
}

void MainWindow::populateRecentProjectsMenu()
{
    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    // Disable recent projects menu when there are no recent projects
    recentProjectsMenu->setEnabled(!recentProjects.isEmpty());

    // Remove existing actions
    recentProjectsMenu->clear();

    // Add action for each recent project
    for (const auto& recentProject : recentProjects) {

        // Get the recent project file path
        const auto recentProjectFilePath = recentProject.toMap()["FilePath"].toString();

        // Check if the recent project exists on disk
        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        // Create recent project action
        auto recentProjectAction = new QAction(recentProjectFilePath);

        // Set action icon + tooltip
        recentProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
        recentProjectAction->setToolTip("Load " + recentProjectFilePath + "(last opened on " + recentProject.toMap()["DateTime"].toDate().toString() + ")");

        // Load the recent project when triggered
        connect(recentProjectAction, &QAction::triggered, this, [recentProjectFilePath]() -> void {
            Application::current()->loadProject(recentProjectFilePath);
        });

        // Add recent project action to the menu
        recentProjectsMenu->addAction(recentProjectAction);
    }
}

}
}
