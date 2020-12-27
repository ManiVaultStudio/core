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

private: // Docking

    /** Sets up docking */
    void initializeDocking();

    /** Sets up the docking area for view plugins (central widget) */
    void initializeViewPluginsDockingArea();

    /** Sets up the docking area for analysis plugins */
    void initializeAnalysisPluginsDockingArea();

    /** Sets up the docking area for settings */
    void initializeSettingsDockingArea();

    /** Sets up the docking area for logging */
    void initializeLoggingDockingArea();
    
private:
    QSharedPointer<Core>            _core;                          /** HDPS core */
    QSharedPointer<Accordion>       _analysisPluginsAccordion;      /** Analysis plugins accordion widget */
    QSharedPointer<DataHierarchy>   _dataHierarchy;                 /** Data hierarchy viewer widget */

private: // Docking
    ads::CDockManager*              _dockManager;                   /** Manager for docking */
    ads::CDockAreaWidget*           _analysisPluginsDockArea;       /** Docking area for analysis plugins */
    ads::CDockAreaWidget*           _viewPluginsDockArea;           /** Docking area for view plugins */
    ads::CDockAreaWidget*           _lastViewPluginDockArea;        /** Docking area for last added view plugin */
    ads::CDockAreaWidget*           _settingsDockArea;              /** Docking area for settings */
    ads::CDockAreaWidget*           _loggingDockArea;               /** Docking area for logging */
    ads::CDockWidget*               _analysisPluginsDockWidget;     /** Dock widget for analysis plugins */
    ads::CDockWidget*               _viewPluginsDockWidget;         /** Dock widget for view plugins */
    ads::CDockWidget*               _settingsDockWidget;            /** Dock widget for settings */
    ads::CDockWidget*               _loggingDockWidget;             /** Dock widget for logging */
};

}
}