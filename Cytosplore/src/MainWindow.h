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
    class ViewPlugin;
    enum Type;
}

namespace gui {

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

    /**
    * Adds a new item to the menu drop-down for this particular type.
    */
    QAction* addMenuAction(plugin::Type type, QString name);

    /**
    * Adds the given view plugin to the main area of the window as a dockable widget.
    */
    void addView(plugin::ViewPlugin* plugin);

private:

public slots:

private slots:

private:
    std::unique_ptr<Core> _core;
};

} // namespace gui

} // namespace hdps

#endif // HDPS_GUI_MAIN_WINDOW_H
