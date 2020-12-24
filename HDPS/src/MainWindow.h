#pragma once

#include "ui_MainWindow.h"
#include "Core.h"
#include "LogDockWidget.h"
#include "DataHierarchy.h"
#include "widgets/Accordion.h"

#include <QMainWindow>
#include <QAction>
#include <QSharedPointer>

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

class LogDockWidget;
class DataHierarchy;

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

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
    * Allows access to the core, which is created by this MainWindow. Useful for
    * unit tests that may need to call addData and requestData.
    */
    CoreInterface& getCore() { return *_core;  }

public: // Adding plugins

    /**
     * Dock (visible) plugin to the window
     * @param plugin Plugin to add (view or analysis)
     */
    void addPlugin(plugin::Plugin* plugin);

private: // Docking

    /**
     * TODO
     */
    void addDockWidget(QWidget* widget, const QString& windowTitle, const ads::DockWidgetArea& dockWidgetArea, ads::CDockAreaWidget* dockAreaWidget = nullptr);

private: // Window geometry persistence

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

    /** Restores the window geometry (position and size) from the application settings */
    void restoreWindowGeometryFromSettings();

    /** Save the window geometry (position and size) to application settings */
    void saveWindowGeometryToSettings();

    /**
     * Applies default window geometry (center position and resized with \p coverage)
     * @param coverage Fraction of the total screen size
     */
    void setDefaultWindowGeometry(const float& coverage = 0.7f);

private:
    QSharedPointer<Core>            _core;                  /** HDPS core */
    QSharedPointer<LogDockWidget>   _logDockWidget;         /** HDPS log dock widget */
    QSharedPointer<DataHierarchy>   _dataHierarchy;         /** Data hierarchy viewer */

private: // Docking
    ads::CDockManager*          _dockManager;                   /** ADS Manager for docking */
    Accordion*                  _analysisPluginsAccordion;      /** Analysis plugin accordion */
    ads::CDockWidget*           _analysisPluginsDockWidget;     /** Analysis plugin dock widget */
    ads::CDockAreaWidget*       _viewPluginsDockArea;           /** ADS Docking area for view plugins */
    ads::CDockAreaWidget*       _settingsDockArea;              /** ADS Settings docking area */
    ads::CDockWidget*           _viewPluginsDockWidget;         /** ADS Central docking widget */
    
};

}
}