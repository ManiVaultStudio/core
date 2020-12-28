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
#include <QDebug>

#include "DockWidgetTab.h"

namespace hdps
{

namespace gui
{

MainWindow::MainWindow(QWidget *parent /*= nullptr*/) :
    QMainWindow(parent),
    _core(nullptr),
    _analysisPluginsAccordion(QSharedPointer<Accordion>::create()),
    _dataHierarchy(nullptr),
    _dockManager(new ads::CDockManager(this)),
    _analysisPluginsDockArea(nullptr),
    _viewPluginsDockArea(nullptr),
    _lastViewPluginDockArea(nullptr),
    _settingsDockArea(nullptr),
    _loggingDockArea(nullptr),
    _analysisPluginsDockWidget(new ads::CDockWidget("Analyses")),
    _viewPluginsDockWidget(new ads::CDockWidget("Central")),
    _settingsDockWidget(new ads::CDockWidget("Settings")),
    _loggingDockWidget(new ads::CDockWidget("Logging"))
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

void MainWindow::addPlugin(plugin::Plugin* plugin)
{
    const auto pluginType = plugin->getType();

    if (pluginType != plugin::Type::ANALYSIS && pluginType != plugin::Type::VIEW)
        return;

    auto dockWidget = new ads::CDockWidget(plugin->getGuiName());

    dockWidget->setIcon(plugin->getIcon());

    switch (pluginType)
    {
        case plugin::Type::ANALYSIS:
        {
            auto analysisPlugin = dynamic_cast<plugin::AnalysisPlugin*>(plugin);

            _analysisPluginsAccordion->addSection(analysisPlugin->getSettings());

            break;
        }

        case plugin::Type::VIEW:
        {
            auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(plugin);

            dockWidget->setWidget(viewPlugin);
            
            auto dockWidgetArea = ads::LeftDockWidgetArea;

            switch (viewPlugin->getDockingLocation())
            {
                case plugin::ViewPlugin::DockingLocation::Left:
                    dockWidgetArea = ads::LeftDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Right:
                    dockWidgetArea = ads::RightDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Top:
                    dockWidgetArea = ads::TopDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Bottom:
                    dockWidgetArea = ads::BottomDockWidgetArea;
                    break;

                case plugin::ViewPlugin::DockingLocation::Center:
                    dockWidgetArea = ads::CenterDockWidgetArea;
                    break;

                default:
                    break;
            }

            if (_lastViewPluginDockArea == nullptr)
                dockWidgetArea = ads::CenterDockWidgetArea;

            _lastViewPluginDockArea = _dockManager->addDockWidget(dockWidgetArea, dockWidget, _lastViewPluginDockArea ? _lastViewPluginDockArea : _viewPluginsDockArea);

            _lastViewPluginDockArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
            _viewPluginsDockWidget->hide();
            _viewPluginsDockWidget->tabWidget()->setVisible(false);
            break;
        }

        default:
            break;
    }
}

void MainWindow::addDockWidget(QWidget* widget, const QString& windowTitle, const ads::DockWidgetArea& dockWidgetArea, ads::CDockAreaWidget* dockAreaWidget)
{
    auto dockWidget = new ads::CDockWidget(windowTitle);

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

void MainWindow::initializeDocking()
{
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewIsDynamic, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewHasWindowFrame, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);

    initializeViewPluginsDockingArea();
    initializeAnalysisPluginsDockingArea();
    initializeSettingsDockingArea();
    initializeLoggingDockingArea();
}

void MainWindow::initializeViewPluginsDockingArea()
{
    _viewPluginsDockArea = _dockManager->setCentralWidget(_viewPluginsDockWidget);

    _viewPluginsDockWidget->tabWidget()->setVisible(false);
}

void MainWindow::initializeAnalysisPluginsDockingArea()
{
    _analysisPluginsDockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    _analysisPluginsDockWidget->setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    _analysisPluginsDockWidget->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    _analysisPluginsDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("sliders-h"));
    _analysisPluginsDockWidget->setWidget(_analysisPluginsAccordion.get());

    _analysisPluginsDockArea = _dockManager->addDockWidget(ads::LeftDockWidgetArea, _analysisPluginsDockWidget);

    //_analysisPluginsDockArea->setDockAreaFlag(ads::CDockAreaWidget::HideSingleWidgetTitleBar, true);
    _analysisPluginsDockArea->setMinimumWidth(300);
    _analysisPluginsDockArea->setMaximumWidth(600);
    _analysisPluginsDockArea->resize(QSize(400, 0));
    
}

void MainWindow::initializeSettingsDockingArea()
{
    _settingsDockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    _settingsDockWidget->setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    _settingsDockWidget->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    _settingsDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("cogs"));

    _settingsDockWidget->setWidget(_dataHierarchy.get());

    _settingsDockArea = _dockManager->addDockWidget(ads::RightDockWidgetArea, _settingsDockWidget);

    //_settingsDockArea->setDockAreaFlag(ads::CDockAreaWidget::HideSingleWidgetTitleBar, true);
    _settingsDockArea->setMinimumWidth(200);
    _settingsDockArea->setMaximumWidth(400);
    _settingsDockArea->resize(QSize(300, 0));
}

void MainWindow::initializeLoggingDockingArea()
{
    _loggingDockWidget->setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("scroll"));

    _loggingDockArea = _dockManager->addDockWidget(ads::DockWidgetArea::BottomDockWidgetArea, _loggingDockWidget, _viewPluginsDockArea);

    _loggingDockArea->hide();
    _loggingDockArea->setMinimumHeight(100);
    _loggingDockArea->setMaximumHeight(300);
    _loggingDockArea->resize(QSize(0, 150));
}

}
}