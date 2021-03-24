#include "MainWindow.h"

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

// Graphics capability checking
#include <QOpenGLFunctions>
#include <QOffscreenSurface>
#include <QMessageBox>
#include <QTimer>

using namespace ads;

namespace hdps
{

namespace gui
{

MainWindow::MainWindow(QWidget *parent /*= nullptr*/) :
    QMainWindow(parent),
    _core(nullptr),
    _analysisPluginsAccordion(QSharedPointer<Accordion>::create()),
    _dataHierarchy(nullptr),
    _dockManager(new CDockManager(this)),
    _analysisPluginsDockArea(nullptr),
    _centralDockArea(nullptr),
    _settingsDockArea(nullptr),
    _loggingDockArea(nullptr),
    _analysisPluginsDockWidget(new CDockWidget("Analyses")),
    _centralDockWidget(new CDockWidget("Views")),
    _settingsDockWidget(new CDockWidget("Settings")),
    _loggingDockWidget(new CDockWidget("Logging"))
{
    setupUi(this);

    _core = QSharedPointer<Core>::create(*this);
    _core->init();

    _dataHierarchy = QSharedPointer<DataHierarchy>::create(_core->getDataManager());

    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    Logger::Initialize();

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
        } else {
            delete _loggingDockWidget->takeWidget();
            _loggingDockArea->hide();
        }
    });

    connect(&_core->getDataManager(), &DataManager::dataChanged, _dataHierarchy.get(), &DataHierarchy::updateDataModel);

    initializeDocking();
    restoreWindowGeometryFromSettings();

    // Delay execution till the event loop has started, otherwise we cannot quit the application
    QTimer::singleShot(1000, this, &MainWindow::checkGraphicsCapabilities);
}

QAction* MainWindow::addImportOption(QString menuName)
{
    return importDataFileMenu->addAction(menuName);
}

QAction* MainWindow::addExportOption(QString menuName)
{
    return exportDataFileMenu->addAction(menuName);
}

QAction* MainWindow::addMenuAction(plugin::Type type, QString name)
{
    switch (type)
    {
        case plugin::Type::ANALYSIS:
            return menuAnalysis->addAction(name);

        case plugin::Type::VIEW:
            return menuVisualization->addAction(name);

        default:
            return nullptr;
    }
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

            _analysisPluginsAccordion->addSection(analysisPlugin->getSettings());

            break;
        }

        case plugin::Type::VIEW:
        {
            auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(plugin);

            dockWidget->setWidget(viewPlugin, CDockWidget::ForceNoScrollArea);
            dockWidget->setProperty("PluginType", "View");
            //dockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);

            connect(viewPlugin, &QWidget::windowTitleChanged, [this, dockWidget](const QString& title) {
                dockWidget->setWindowTitle(title);
            });

            auto dockWidgetArea = LeftDockWidgetArea;

            switch (viewPlugin->getDockingLocation())
            {
                case plugin::ViewPlugin::DockingLocation::Left:
                    dockWidgetArea = LeftDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Right:
                    dockWidgetArea = RightDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Top:
                    dockWidgetArea = TopDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Bottom:
                    dockWidgetArea = BottomDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Center:
                    dockWidgetArea = CenterDockWidgetArea;
                    break;

                default:
                    break;
            }

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
    initializeAnalysisPluginsDockingArea();
    initializeSettingsDockingArea();
    initializeLoggingDockingArea();

    QObject::connect(_dockManager, &CDockManager::dockAreasAdded, this, &MainWindow::updateCentralWidgetVisibility);
    QObject::connect(_dockManager, &CDockManager::dockAreasRemoved, this, &MainWindow::updateCentralWidgetVisibility);
}

void MainWindow::initializeCentralDockingArea()
{
    QLabel* welcomeLabel = new QLabel();

    // choose the icon for different-dpi screens
    const int pixelRatio = static_cast<int>(std::ceil(devicePixelRatio()));
    QString iconName = ":/Icons/AppIcon256";
    if (pixelRatio > 1) iconName = ":/Icons/AppIcon512";
    if (pixelRatio > 2) iconName = ":/Icons/AppIcon1024";

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

void MainWindow::initializeAnalysisPluginsDockingArea()
{
    _analysisPluginsDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    _analysisPluginsDockWidget->setFeature(CDockWidget::DockWidgetFloatable, true);
    _analysisPluginsDockWidget->setFeature(CDockWidget::DockWidgetMovable, true);
    _analysisPluginsDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("sliders-h"));
    _analysisPluginsDockWidget->setWidget(_analysisPluginsAccordion.get());

    _analysisPluginsDockArea = _dockManager->addDockWidget(LeftDockWidgetArea, _analysisPluginsDockWidget);

    //_analysisPluginsDockArea->setDockAreaFlag(CDockAreaWidget::HideSingleWidgetTitleBar, true);
    _analysisPluginsDockArea->setMinimumWidth(300);
    //_analysisPluginsDockArea->setMaximumWidth(600);
    _analysisPluginsDockArea->resize(QSize(400, 0));
}

void MainWindow::initializeSettingsDockingArea()
{
    _settingsDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    _settingsDockWidget->setFeature(CDockWidget::DockWidgetFloatable, true);
    _settingsDockWidget->setFeature(CDockWidget::DockWidgetMovable, true);
    _settingsDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cogs"));

    _settingsDockWidget->setWidget(_dataHierarchy.get());

    _settingsDockArea = _dockManager->addDockWidget(RightDockWidgetArea, _settingsDockWidget);

    //_settingsDockArea->setDockAreaFlag(CDockAreaWidget::HideSingleWidgetTitleBar, true);
    _settingsDockArea->setMinimumWidth(200);
    //_settingsDockArea->setMaximumWidth(400);
    _settingsDockArea->resize(QSize(300, 0));
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
        _centralDockArea->dockWidget(0)->toggleView(true);
    }
    else {
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

}
}