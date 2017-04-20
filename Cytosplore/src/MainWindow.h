#ifndef HDPS_GUI_MAIN_WINDOW_H
#define HDPS_GUI_MAIN_WINDOW_H

#include <QMainWindow>



namespace Ui
{
    class MainWindow;
}

namespace hdps {

namespace plugin {
    class PluginManager;
}

namespace gui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

private:

public slots:

private slots:

private:

	QSharedPointer<Ui::MainWindow> _ui;
    
    QSharedPointer<plugin::PluginManager> _pluginManager;
};

} // namespace gui

} // namespace hdps

#endif // HDPS_GUI_MAIN_WINDOW_H
