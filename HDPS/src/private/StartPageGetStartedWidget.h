#pragma once

#include "StartPageActionsWidget.h"

#include <QWidget>

/**
 * Start page get started widget class
 *
 * Widget class which contains actions related to getting started.
 *
 * @author Thomas Kroes
 */
class StartPageGetStartedWidget : public QWidget
{
protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageGetStartedWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

private:
    StartPageActionsWidget  _createProjectFromWorkspaceWidget;  /** Actions widget for creating a project from workspace */
    StartPageActionsWidget  _createProjectFromDatasetWidget;    /** Actions widget for creating a project from a dataset */

    friend class StartPageContentWidget;
};