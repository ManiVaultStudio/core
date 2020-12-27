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

namespace hdps
{

namespace gui
{

MainWindow::MainWindow(QWidget *parent /*= nullptr*/) :
    QMainWindow(parent),
    _core(nullptr),
    _logDockWidget(nullptr),
    _dataHierarchy(nullptr),
    _dockManager(new ads::CDockManager(this)),
    _analysisPluginsDockArea(nullptr),
    _viewPluginsDockArea(nullptr),
    _settingsDockArea(nullptr),
    _analysisPluginsDockWidget(new ads::CDockWidget("Analysis")),
    _viewPluginsDockWidget(new ads::CDockWidget("Central Dock Widget")),
    _analysisPluginsAccordion(new Accordion(this))
{
    setupUi(this);

    // Create and initialize HDPS core
    _core = QSharedPointer<Core>::create(*this);
    _core->init();

    //_dataHierarchy = QSharedPointer<DataHierarchy>::create(_core->getDataManager());

    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    Logger::Initialize();

    QObject::connect(findLogFileAction, &QAction::triggered, [this](bool)
    {
        const auto filePath = Logger::GetFilePathName();

        if (!hdps::util::ShowFileInFolder(filePath))
        {
            QMessageBox::information(this,
                QObject::tr("Log file not found"),
                QObject::tr("The log file is not found:\n%1").arg(filePath));
        }
    });

    QObject::connect(logViewAction, &QAction::triggered, [this](const bool checked)
    {
        if (checked)
        {
            _logDockWidget = QSharedPointer<LogDockWidget>::create(*this);
            
            //addDockWidget(Qt::BottomDockWidgetArea, TODO, _logDockWidget.get(), Qt::Horizontal);
        }
        else
        {
            //removeDockWidget(_logDockWidget.get());
            //_logDockWidget.reset();
        }
    });

    connect(&_core->getDataManager(), &DataManager::dataChanged, _dataHierarchy.get(), &DataHierarchy::updateDataModel);
    // Note: addWidget takes the ownership of its argument, so it should be released from the unique_ptr.
    //_settingsWidget->addWidget(dataHierarchy.get());
    //_dataHierarchy = dataHierarchy.release();

    // Advanced docking system
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewIsDynamic, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewShowsContentPixmap, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::DragPreviewHasWindowFrame, true);
    ads::CDockManager::setConfigFlag(ads::CDockManager::OpaqueSplitterResize, false);
    ads::CDockManager::setConfigFlag(ads::CDockManager::FocusHighlighting, true);

    _viewPluginsDockArea = _dockManager->setCentralWidget(_viewPluginsDockWidget);
    
    _analysisPluginsDockWidget->setWidget(_analysisPluginsAccordion);

    _analysisPluginsDockArea = _dockManager->addDockWidget(ads::LeftDockWidgetArea, _analysisPluginsDockWidget);

    _analysisPluginsDockArea->setMinimumWidth(200);
    _analysisPluginsDockArea->setMaximumWidth(600);
    _analysisPluginsDockArea->resize(QSize(400, 0));
    
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

            auto settingsWidget = analysisPlugin->getSettings();

            settingsWidget->setWindowTitle(plugin->getGuiName());
            settingsWidget->setWindowIcon(plugin->getIcon());

            _analysisPluginsAccordion->addSection(settingsWidget);

            break;
        }

        case plugin::Type::VIEW:
        {
            auto viewPlugin = dynamic_cast<plugin::ViewPlugin*>(plugin);

            dockWidget->setWidget(viewPlugin, ads::CDockWidget::ForceNoScrollArea);
            
            _dockManager->addDockWidget(ads::LeftDockWidgetArea, dockWidget, _viewPluginsDockArea);

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

}

}