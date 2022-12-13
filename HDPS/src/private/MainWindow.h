#pragma once

#include "Core.h"

#include "FileMenu.h"
#include "ViewMenu.h"
#include "HelpMenu.h"

#include "StartPageWidget.h"
#include "ProjectWidget.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QPointer>

class MainWindow : public QMainWindow
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

private:
    Core                        _core;                  /** Instance of the core */
    QPointer<FileMenu>          _fileMenu;              /** File menu */
    QPointer<ViewMenu>          _viewMenu;              /** View menu */
    QPointer<HelpMenu>          _helpMenu;              /** Help menu */
    QPointer<QStackedWidget>    _stackedWidget;         /** Stacked widgets (0: start page widget, 1: workspace widget) */
    QPointer<StartPageWidget>   _startPageWidget;       /** Start page widget which resides in the stacked widget */
    QPointer<ProjectWidget>     _projectWidget;         /** Project widget which resides in the stacked widget */
};
