#pragma once

#include "ui_MainWindow.h"
#include "Core.h"

#include <QMainWindow>
#include <QAction>
#include <QSharedPointer>
#include <QLabel>

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

protected: // Menu

    /** Setup file menu */
    void setupFileMenu();

    /** Fill the recent projects menu with entries */
    void populateRecentProjectsMenu();

private:
    Core        _core;                      /** Instance of the core */
    QMenu       _loadedViewPluginsMenu;     /** Menu for loaded view plugins */
    QMenu       _pluginsHelpMenu;           /** Menu for loaded plugins help */
};
