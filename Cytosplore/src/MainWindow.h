#ifndef MAIN_WINDOW
#define MAIN_WINDOW

#include <QMainWindow>

class MCV_PluginManager;

namespace Ui
{
    class MainWindow;
}

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
    
    QSharedPointer<MCV_PluginManager> _pluginManager;
};

#endif // MAIN_WINDOW
