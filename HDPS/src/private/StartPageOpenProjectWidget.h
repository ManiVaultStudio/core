#pragma once

#include "StartPageActionsWidget.h"

#include <actions/RecentFilesAction.h>

#include <QWidget>

/**
 * Start page open project widget class
 *
 * Widget class which contains actions related to opening existing projects.
 *
 * @author Thomas Kroes
 */
class StartPageOpenProjectWidget : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageOpenProjectWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

private:
    StartPageActionsWidget          _openProjectWidget;         /** Actions widget for open project action */
    StartPageActionsWidget          _recentProjectsWidget;      /** Actions widget for existing projects action */
    hdps::gui::RecentFilesAction    _recentProjectsAction;      /** Action for recent projects */

    friend class StartPageContentWidget;
};