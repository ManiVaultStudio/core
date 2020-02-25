#pragma once

#include "ui_MainWindow.h"
#include "Core.h"
#include "CentralWidget.h"

#include <QMainWindow>
#include <QAction>
#include <QByteArray>

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
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    std::unique_ptr<Core> _core;
    std::unique_ptr<LogDockWidget> _logDockWidget;

    CentralWidget* _centralWidget;
    DataHierarchy* _dataHierarchy;
    std::unique_ptr<SettingsWidget> _settingsWidget;

    QByteArray _windowConfiguration;
};

} // namespace gui

} // namespace hdps
