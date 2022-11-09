#pragma once

#include "ui_MainWindow.h"
#include "Core.h"

#include <QMainWindow>
#include <QAction>
#include <QSharedPointer>
#include <QLabel>

// Advanced docking system
#include "DockManager.h"
#include "DockAreaWidget.h"

namespace Ui
{
    class MainWindow;
}

class StartPageWidget;

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    /**
     * Callback invoked when the window shows
     * @param closeEvent Close event
     */
    void showEvent(QShowEvent* showEvent) override;

    /**
     * Callback invoked when the window closes
     * @param closeEvent Close event
     */
    void closeEvent(QCloseEvent* closeEvent) override;

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

    void checkGraphicsCapabilities();

private: // Docking

    /** Sets up docking */
    void initializeDocking();

    /** Sets up the docking area for view plugins (central widget) */
    void initializeCentralDockingArea();

    /** Updates the visibility of the central dock widget (depending on its content) */
    void updateCentralWidget();

    /**
     * Return a list of (open) view plugin dock widgets
     * @param open Whether to only include open dock widgets
     * @return View plugin dock area widgets
     */
    QList<ads::CDockWidget*> getViewPluginDockWidgets(const bool& openOnly = true);

protected: // Menu

    /** Setup file menu */
    void setupFileMenu();

    /** Setup view menu */
    void setupViewMenu();

    /** Fill the recent projects menu with entries */
    void populateRecentProjectsMenu();

private:
    Core                        _core;                          /** Instance of the core */
    StartPageWidget*            _startPageWidget;               /** Pointer to the start page widget */
    ads::CDockManager*          _dockManager;                   /** Manager for docking */
    ads::CDockAreaWidget*       _centralDockArea;               /** Docking area for view plugins */
    ads::CDockAreaWidget*       _lastDockAreaWidget;            /** Last docking area widget (if any) */
    ads::CDockWidget*           _centralDockWidget;             /** Dock widget for view plugins */
    ads::CDockWidget*           _startPageDockWidget;           /** Dock widget for the start page */
    QMenu                       _loadedViewPluginsMenu;         /** Menu for loaded view plugins */
    QMenu                       _pluginsHelpMenu;               /** Menu for loaded plugins help */
};
