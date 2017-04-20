#ifndef HDPS_GUI_MAIN_WINDOW_H
#define HDPS_GUI_MAIN_WINDOW_H

#include <QMainWindow>
#include "ui_MainWindow.h"

#include <QAction>

#include "Core.h"

namespace Ui
{
    class MainWindow;
}

namespace hdps {

namespace plugin {
    class PluginManager;
    enum Type;
}

namespace gui {

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

    QAction* addMenuAction(plugin::Type type, QString name);

private:

public slots:

private slots:

private:
    std::unique_ptr<Core> _core;
};

} // namespace gui

} // namespace hdps

#endif // HDPS_GUI_MAIN_WINDOW_H
