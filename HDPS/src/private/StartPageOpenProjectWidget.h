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

    /**
     * Create icon for default project with \p default view alignment and set \p icon by reference
     * @param alignment Alignment of the project
     * @param icon Icon that is set by reference
     */
    void createIconForDefaultProject(const Qt::Alignment& alignment, QIcon& icon);

private:
    StartPageActionsWidget          _openCreateProjectWidget;   /** Actions widget for open and create project action */
    StartPageActionsWidget          _recentProjectsWidget;      /** Actions widget for existing projects action */
    StartPageActionsWidget          _exampleProjectsWidget;     /** Actions widget for example projects action */
    hdps::gui::RecentFilesAction    _recentProjectsAction;      /** Action for recent projects */
    QIcon                           _leftAlignedIcon;           /** Icon for left-aligned default project */
    QIcon                           _rightAlignedIcon;          /** Icon for right-aligned default project */

    friend class StartPageContentWidget;
};