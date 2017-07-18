#include "MainWindow.h"

#include "PluginManager.h"
#include "PluginType.h"

#include "ViewPlugin.h"
#include "widgets/SettingsWidget.h"

#include <QDebug>

namespace hdps {

namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUi(this);
    
    _core = std::unique_ptr<Core>(new Core(*this));
    _core->init();
}

MainWindow::~MainWindow()
{
}

QAction* MainWindow::addMenuAction(plugin::Type type, QString name) {
    switch (type) {
    case plugin::Type::ANALYSIS:      return menuAnalysis->addAction(name);
    case plugin::Type::LOADER:        return menuFile->addAction(name);
    case plugin::Type::WRITER:        return menuFile->addAction(name);
    case plugin::Type::TRANFORMATION: return menuTransformation->addAction(name);
    case plugin::Type::VIEW:          return menuView->addAction(name);
    default: return nullptr;
    }
}

void MainWindow::addView(plugin::ViewPlugin* plugin) {
    addDockWidget(Qt::RightDockWidgetArea, plugin);
}

void MainWindow::addSettings(gui::SettingsWidget* settings) {
    addDockWidget(Qt::LeftDockWidgetArea, settings);
}

} // namespace gui

} // namespace hdps
