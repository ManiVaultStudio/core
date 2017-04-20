#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "PluginManager.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>

namespace hdps {

namespace gui {

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(NULL)
{
    _ui = QSharedPointer<Ui::MainWindow>(new Ui::MainWindow());
    _ui->setupUi(this);
    
    _pluginManager = QSharedPointer<plugin::PluginManager>(new plugin::PluginManager());
    _pluginManager->LoadPlugins(_ui);
}

MainWindow::~MainWindow()
{
}

} // namespace gui

} // namespace hdps
