#ifndef HDPS_MAINWINDOW_H
#define HDPS_MAINWINDOW_H

#include "ui_MainWindow.h"
#include "Core.h"
#include "CentralWidget.h"

#include <QMainWindow>
#include <QAction>
#include <QByteArray>

// Advanced docking system
#include "DockManager.h"
#include "DockAreaWidget.h"

namespace Ui
{
    class MainWindow;
}

namespace hdps
{

namespace plugin
{
    class PluginManager;
    class ViewPlugin;
    enum class Type;
}

namespace gui
{

class SettingsWidget;
class LogDockWidget;
class DataHierarchy;

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
	~MainWindow() override;

    /**
    * Adds a new item to the import menu.
    */
    QAction* addImportOption(QString menuName);

    /**
    * Adds a new item to the export menu.
    */
    QAction* addExportOption(QString menuName);

    /**
    * Adds a new item to the menu drop-down for this particular type.
    */
    QAction* addMenuAction(plugin::Type type, QString name);

    /**
    * Adds the given view plugin to the main area of the window as a dockable widget.
    */
    void addView(plugin::ViewPlugin* plugin);

    /**
    * Adds the given settings widget to the settings area of the window.
    */
    void addSettings(SettingsWidget* settings);

    /**
     * Moves the window to the center of the screen and resizes it to a fraction
     * of the total screen size defined by 'coverage'.
     */
    void centerAndResize(float coverage);

    /**
    * Allows access to the core, which is created by this MainWindow. Useful for
    * unit tests that may need to call addData and requestData.
    */
    CoreInterface& getCore() { return *_core;  }

public slots:
    /**
    * Store the current window layout so we can restore it later
    */
    void storeLayout();

    /**
    * Restore the configuration of dockable widgets
    */
    void restoreLayout();

private:
    

private: // Window geometry persistence

    /** Save window position and size to application settings */
    void saveGeometryToSettings();

    /**
     * Invoked when the window position changes
     * @param moveEvent Move event that occurred
     */
    void moveEvent(QMoveEvent* moveEvent) override;

    /**
     * Invoked when the window size changes
     * @param resizeEvent Resize event that occurred
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

private:
    std::unique_ptr<Core>               _core;                  /** HDPS core */
    std::unique_ptr<LogDockWidget>      _logDockWidget;         /** HDPS log dock widget */
    DataHierarchy*                      _dataHierarchy;         /** Data hierarchy viewer */
    std::unique_ptr<SettingsWidget>     _settingsWidget;        /** Settings widget */

private: // Advanced docking system
    ads::CDockManager*          _dockManager;               /** ADS Manager for docking */
    ads::CDockAreaWidget*       _centralDockArea;           /** ADS Central docking area */
    ads::CDockWidget*           _centralDockWidget;         /** ADS Central docking widget */
    QList<ads::CDockWidget*>    _viewPluginDockWidgets;     /** ADS Docking area for view plugins */
    ads::CDockAreaWidget*       _analysisDockingArea;       /** ADS Docking area for analysis plugins */
};

} // namespace gui

} // namespace hdps

#endif // HDPS_MAINWINDOW_H
