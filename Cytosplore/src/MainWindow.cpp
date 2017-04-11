#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "MCV_PluginManager.h"

#include <QDebug>
#include <QDir>
#include <QPluginLoader>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _ui(NULL)
{
    _ui = QSharedPointer<Ui::MainWindow>(new Ui::MainWindow());
    _ui->setupUi(this);
    
    _pluginManager = QSharedPointer<MCV_PluginManager>(new MCV_PluginManager());
    _pluginManager->LoadPlugins(_ui);
}

MainWindow::~MainWindow()
{
}
