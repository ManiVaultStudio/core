#pragma once

#include "StartPageOpenProjectWidget.h"
#include "StartPageGetStartedWidget.h"

#include <actions/ToggleAction.h>
#include <actions/OptionsAction.h>

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * Start page content widget class
 *
 * Widget class for showingall the actions.
 *
 * @author Thomas Kroes
 */
class StartPageContentWidget final : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageContentWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

public:

    /**
     * Create header label
     * @param title Title of the header
     * @param tooltip Tooltip of the header
     * @return Pointer to newly created header label
     */
    static QLabel* createHeaderLabel(const QString& title, const QString& tooltip);

public: // Action getters

    const hdps::gui::ToggleAction& getCompactViewAction() const { return _compactViewAction; }
    const hdps::gui::ToggleAction& getToggleOpenCreateProjectAction() const { return _toggleOpenCreateProjectAction; }
    const hdps::gui::ToggleAction& getToggleRecentProjectsAction() const { return _toggleRecentProjectsAction; }
    const hdps::gui::ToggleAction& getToggleExampleProjectsAction() const { return _toggleExampleProjectsAction; }
    const hdps::gui::ToggleAction& getToggleProjectFromWorkspaceAction() const { return _toggleProjectFromWorkspaceAction; }
    const hdps::gui::ToggleAction& getToggleProjectFromDataAction() const { return _toggleProjectFromDataAction; }
    const hdps::gui::ToggleAction& getToggleTutorialVideosAction() const { return _toggleTutorialVideosAction; }

private:
    QVBoxLayout                 _mainLayout;                        /** Main layout */
    QHBoxLayout                 _collumnsLayout;                    /** Columns layout */
    QHBoxLayout                 _toolbarLayout;                     /** Toolbar layout */
    hdps::gui::ToggleAction     _compactViewAction;                 /** Toggle compact view on/off */
    hdps::gui::ToggleAction     _toggleOpenCreateProjectAction;     /** Toggle open and create project section */
    hdps::gui::ToggleAction     _toggleRecentProjectsAction;        /** Toggle recent projects section */
    hdps::gui::ToggleAction     _toggleExampleProjectsAction;       /** Toggle example projects section */
    hdps::gui::ToggleAction     _toggleProjectFromWorkspaceAction;  /** Toggle project from workspace section */
    hdps::gui::ToggleAction     _toggleProjectFromDataAction;       /** Toggle project from data section */
    hdps::gui::ToggleAction     _toggleTutorialVideosAction;        /** Toggle tutorial videos section */
    hdps::gui::GroupAction      _settingsAction;                    /** Settings action */
    StartPageOpenProjectWidget  _openProjectWidget;                 /** Left column widget for opening existing projects */
    StartPageGetStartedWidget   _getStartedWidget;                  /** Right column widget for getting started operations */

    friend class StartPageWidget;
};