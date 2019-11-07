#include "MainWindow.h"

#include "DataManager.h" // To connect changed data signal to dataHierarchy
#include "Logger.h"
#include "LogDockWidget.h"
#include "DataHierarchy.h"
#include "PluginManager.h"
#include "PluginType.h"

#include "ViewPlugin.h"
#include "widgets/SettingsWidget.h"

#include <QApplication> // Used by centerAndResize
#include <QDesktopWidget> // Used by centerAndResize
#include <QDesktopServices>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <QDebug>

namespace
{
    // TODO Move this function to a utility file.
    bool ShowFileInFolder(const QString path)
    {
        const QFileInfo info(path);

        if (!info.exists())
        {
            return false;
        }

        // Based upon: How to "Reveal in Finder" or “Show in Explorer” with Qt
        // https://stackoverflow.com/questions/3490336/how-to-reveal-in-finder-or-show-in-explorer-with-qt
        enum class Os { Windows, Mac, Other };

        constexpr auto os =
#if defined(Q_OS_WIN)
            Os::Windows
#elif defined(Q_OS_MAC)
            Os::Mac
#else
            Os::Other
#endif
            ;

        if (os == Os::Windows)
        {
            const auto args = QStringList{}
                << "/select,"
                << QDir::toNativeSeparators(path);

            if (QProcess::startDetached("explorer.exe", args))
            {
                return true;
            }
        }
        if (os == Os::Mac)
        {
            const auto args = QStringList{}
                << "-e"
                << "tell application \"Finder\""
                << "-e"
                << "activate"
                << "-e"
                << "select POSIX file \"" + path + "\""
                << "-e"
                << "end tell"
                << "-e"
                << "return";

            if (QProcess::execute("/usr/bin/osascript", args) == 0)
            {
                return true;
            }
        }
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.path()));
        return true;
    }

}

namespace hdps
{
namespace gui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    
    _core = std::make_unique<Core>(*this);
    _core->init();

    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    
    Logger::Initialize();

    QObject::connect(findLogFileAction, &QAction::triggered, [this](bool)
    {
        const auto filePath = Logger::GetFilePathName();

        if ( !ShowFileInFolder(filePath))
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
            _logDockWidget = std::make_unique<LogDockWidget>(*this);
            addDockWidget(Qt::BottomDockWidgetArea, _logDockWidget.get(), Qt::Horizontal);
        }
        else
        {
            removeDockWidget(_logDockWidget.get());
            _logDockWidget.reset();
        }
    });

    _centralWidget = new CentralWidget();
    setCentralWidget(_centralWidget);

    _settingsWidget = std::make_unique<SettingsWidget>();
    _settingsWidget->setAllowedAreas(Qt::RightDockWidgetArea);

    _dataHierarchy = std::make_unique<DataHierarchy>(_core->getDataManager());
    connect(&_core->getDataManager(), &DataManager::dataChanged, _dataHierarchy.get(), &DataHierarchy::updateDataModel);
    _settingsWidget->addWidget(_dataHierarchy.get());

    addDockWidget(Qt::RightDockWidgetArea, _settingsWidget.get());
}

MainWindow::~MainWindow()
{
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
    case plugin::Type::ANALYSIS:      return menuAnalysis->addAction(name);
    case plugin::Type::VIEW:          return menuVisualization->addAction(name);
    default: return nullptr;
    }
}

void MainWindow::addView(plugin::ViewPlugin* plugin)
{
    _centralWidget->addView(plugin);
}

void MainWindow::addSettings(gui::SettingsWidget* settings)
{
    addDockWidget(Qt::LeftDockWidgetArea, settings);
}

void MainWindow::centerAndResize(float coverage) {
    // Retrieve the dimensions available on the current screen
    QSize availableSize = qApp->desktop()->availableGeometry().size();
    QSize newSize(availableSize.width() * coverage, availableSize.height() * coverage);

    qDebug() << "Available screen size " << availableSize.width() << "x" << availableSize.height();
    qDebug() << "Initial application size " << newSize.width() << "x" << newSize.height();

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newSize, qApp->desktop()->availableGeometry()));
}

void MainWindow::storeLayout()
{
    _windowConfiguration = saveState();
}

void MainWindow::restoreLayout()
{
    restoreState(_windowConfiguration);
}

void MainWindow::changeEvent(QEvent *event)
{
    storeLayout();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    storeLayout();
}

void MainWindow::hideEvent(QHideEvent *event)
{
    storeLayout();
}

void MainWindow::showEvent(QShowEvent *event)
{
    restoreLayout();

    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
    for (auto child : dockWidgets)
        child->setVisible(true);
}

} // namespace gui

} // namespace hdps
