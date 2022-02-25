#include "MainWindow.h"
#include "DataHierarchyWidget.h"
#include "DataPropertiesWidget.h"
#include "DataManager.h" // To connect changed data signal to dataHierarchy
#include "Logger.h"
#include "PluginManager.h"
#include "PluginType.h"
#include "Application.h"
#include "ViewPlugin.h"
#include "AnalysisPlugin.h"

#include "util/FileUtil.h"

#include <QDesktopWidget>
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

using namespace ads;

namespace hdps
{

namespace gui
{

MainWindow::MainWindow(QWidget *parent /*= nullptr*/) :
    QMainWindow(parent),
    _dataHierarchyWidget(nullptr),
    _dataPropertiesWidget(nullptr),
    _dockManager(new CDockManager(this)),
    _centralDockArea(nullptr),
    _settingsDockArea(nullptr),
    _loggingDockArea(nullptr),
    _centralDockWidget(new CDockWidget("Views")),
    _dataHierarchyDockWidget(new CDockWidget("Data hierarchy")),
    _dataPropertiesDockWidget(new CDockWidget("Data properties")),
    _loggingDockWidget(new CDockWidget("Logging"))
{
    setupUi(this);

    _core = QSharedPointer<Core>::create(*this);

    dynamic_cast<Application*>(qApp)->setCore(_core.get());

    _core->init();

    _dataHierarchyWidget    = new DataHierarchyWidget(this);
    _dataPropertiesWidget   = new DataPropertiesWidget(this);

    // Change the window title when the current project file changed
    connect(Application::current(), &Application::currentProjectFilePathChanged, [this](const QString& currentProjectFilePath) {
        setWindowTitle(currentProjectFilePath + (currentProjectFilePath.isEmpty() ? " HDPS" : " - HDPS"));
    });

    // Setup menus
    setupFileMenu();
    setupViewMenu();

    initializeDocking();
    restoreWindowGeometryFromSettings();

    Logger::Initialize();

    // Delay execution till the event loop has started, otherwise we cannot quit the application
    QTimer::singleShot(1000, this, &MainWindow::checkGraphicsCapabilities);
}

QAction* MainWindow::addImportOption(const QString& actionName, const QIcon& icon)
{
    return importDataMenu->addAction(icon, actionName);
}

QAction* MainWindow::addViewAction(const plugin::Type& type, const QString name, const QIcon& icon)
{
    return menuVisualization->addAction(icon, name);
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
            //dockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);

            connect(&viewPlugin->getWidget(), &QWidget::windowTitleChanged, [this, dockWidget](const QString& title) {
                dockWidget->setWindowTitle(title);
            });

            auto dockWidgetArea = LeftDockWidgetArea;

            if (getViewPluginDockWidgets().isEmpty())
                dockWidgetArea = CenterDockWidgetArea;

            _dockManager->addDockWidget(dockWidgetArea, dockWidget, _centralDockArea);
            
            QObject::connect(dockWidget->dockAreaWidget(), &CDockAreaWidget::currentChanged, [this](int index) {
                updateCentralWidgetVisibility();
            });
            
            QObject::connect(dockWidget, &CDockWidget::closed, [this, dockWidget]() {
                _dockManager->removeDockWidget(dockWidget);
                updateCentralWidgetVisibility();
            });

            QObject::connect(dockWidget, &CDockWidget::topLevelChanged, [this, dockWidget](bool topLevel) {
                updateCentralWidgetVisibility();
            });
            
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
    initializeSettingsDockingArea();
    initializeLoggingDockingArea();

    connect(_dockManager, &CDockManager::dockAreasAdded, this, &MainWindow::updateCentralWidgetVisibility);
    connect(_dockManager, &CDockManager::dockAreasRemoved, this, &MainWindow::updateCentralWidgetVisibility);
}

void MainWindow::initializeCentralDockingArea()
{
    QLabel* welcomeLabel = new QLabel();

    // choose the icon for different-dpi screens
    const int pixelRatio = devicePixelRatio();
    QString iconName = ":/Images/AppBackground256";
    if (pixelRatio > 1) iconName = ":/Images/AppBackground512";
    if (pixelRatio > 2) iconName = ":/Images/AppBackground1024";
    
    welcomeLabel->setPixmap(QPixmap(iconName).scaled(256, 256));
    welcomeLabel->setAlignment(Qt::AlignCenter);

    _centralDockWidget->setWidget(welcomeLabel);

    _centralDockArea = _dockManager->setCentralWidget(_centralDockWidget);
   
    _centralDockWidget->setFeature(CDockWidget::DockWidgetClosable, true);
    _centralDockWidget->setFeature(CDockWidget::DockWidgetFloatable, true);
    _centralDockWidget->setFeature(CDockWidget::DockWidgetMovable, true);
    _centralDockWidget->setFeature(CDockWidget::NoTab, true);
    _centralDockWidget->tabWidget()->setVisible(false);
}

void MainWindow::initializeSettingsDockingArea()
{
    _dataHierarchyDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    _dataHierarchyDockWidget->setFeature(CDockWidget::DockWidgetFloatable, true);
    _dataHierarchyDockWidget->setFeature(CDockWidget::DockWidgetMovable, true);
    _dataHierarchyDockWidget->setIcon(Application::getIconFont("FontAwesome").getIcon("sitemap", QSize(16, 16)));
    _dataHierarchyDockWidget->setWidget(_dataHierarchyWidget);

    _dataPropertiesDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    _dataPropertiesDockWidget->setFeature(CDockWidget::DockWidgetFloatable, true);
    _dataPropertiesDockWidget->setFeature(CDockWidget::DockWidgetMovable, true);
    _dataPropertiesDockWidget->setIcon(Application::getIconFont("FontAwesome").getIcon("edit", QSize(16, 16)));
    _dataPropertiesDockWidget->setWidget(_dataPropertiesWidget);

    _settingsDockArea = _dockManager->addDockWidget(RightDockWidgetArea, _dataHierarchyDockWidget);
    _settingsDockArea = _dockManager->addDockWidget(BottomDockWidgetArea, _dataPropertiesDockWidget, _settingsDockArea);

    auto splitter = ads::internal::findParent<ads::CDockSplitter*>(_settingsDockArea);

    if (splitter != nullptr) {
        const auto height = splitter->height();
        splitter->setSizes({ height * 1 / 3, height * 2 / 3 });
    }

    _settingsDockArea->setMinimumWidth(500);
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

void MainWindow::updateCentralWidgetVisibility()
{
    if (getViewPluginDockWidgets().count() == 0) {
        _centralDockArea->setAllowedAreas(DockWidgetArea::CenterDockWidgetArea);
        _centralDockArea->dockWidget(0)->toggleView(true);
    }
    else {
        _centralDockArea->setAllowedAreas(DockWidgetArea::AllDockAreas);

        if (_centralDockArea->dockWidgets().size() == 1)
            _centralDockArea->dockWidget(0)->toggleView(false);
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
    resetDataModelAction->setIcon(Application::getIconFont("FontAwesome").getIcon("undo"));
    importDataMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    exitAction->setIcon(Application::getIconFont("FontAwesome").getIcon("sign-out-alt"));

    // Set action tooltips
    openProjectAction->setToolTip("Open project from disk");
    saveProjectAction->setToolTip("Save project to disk");
    saveProjectAsAction->setToolTip("Save project to disk in a chosen location");
    recentProjectsMenu->setToolTip("Recently opened HDPS projects");
    resetDataModelAction->setToolTip("Reset the data model");
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
    connect(resetDataModelAction, &QAction::triggered, this, [this]() -> void {

        // Get all loaded datasets (irrespective of the data type)
        const auto loadedDatasets = _core->requestAllDataSets();

        // The project needs to be cleared if there are one or more datasets loaded
        if (!loadedDatasets.empty()) {

            // Check in the settings if the user has to be prompted with a question whether to automatically remove all datasets
            if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {

                // Ask for confirmation dialog
                DataRemoveAction::ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Reset the data model", loadedDatasets);

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
        resetDataModelAction->setEnabled(hasDatasets);

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
    // Get recent projects
    const auto recentProjects = Application::current()->getSetting("Projects/Recent", QVariantList()).toList();

    // Disable recent projects menu when there are no recent projects
    recentProjectsMenu->setEnabled(!recentProjects.isEmpty());

    // Remove existing actions
    recentProjectsMenu->clear();

    // Get recent projects from settings
    for (const auto& recentProject : recentProjects) {

        // Get the recent project file path
        const auto recentProjectFilePath = recentProject.toString();

        // Check if the recent project exists on disk
        if (!QFileInfo(recentProjectFilePath).exists())
            continue;

        // Create recent project action
        auto recentProjectAction = new QAction(recentProjectFilePath);

        // Set action icon tooltip
        recentProjectAction->setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
        recentProjectAction->setToolTip("Load " + recentProjectFilePath);

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
