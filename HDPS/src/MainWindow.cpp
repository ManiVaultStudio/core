#include "MainWindow.h"

#include "PluginManager.h"
#include "PluginType.h"

#include "ViewPlugin.h"
#include "widgets/SettingsWidget.h"

#include <QDebug>

namespace hdps
{
namespace gui
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    
    _core = std::unique_ptr<Core>(new Core(*this));
    _core->init();

    QObject::connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
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
    addDockWidget(Qt::RightDockWidgetArea, plugin);
}

void MainWindow::addSettings(gui::SettingsWidget* settings)
{
    addDockWidget(Qt::LeftDockWidgetArea, settings);
}

} // namespace gui

} // namespace hdps
