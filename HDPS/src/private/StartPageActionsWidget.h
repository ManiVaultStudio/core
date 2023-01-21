#pragma once

#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <widgets/HierarchyWidget.h>

#include <QWidget>

/**
 * Start page actions widget class
 *
 * Widget class for listing start page actions.
 *
 * @author Thomas Kroes
 */
class StartPageActionsWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageActionsWidget(QWidget* parent = nullptr);

    /**
     * Get model
     * @return Reference to model
     */
    StartPageActionsModel& getModel();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    StartPageActionsFilterModel& getFilterModel();

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    hdps::gui::HierarchyWidget& getHierarchyWidget();

    void createEditors();

private:
    StartPageActionsModel           _model;             /** Model which contains start page actions */
    StartPageActionsFilterModel     _filterModel;       /** Model for filtering and sorting start page actions */
    hdps::gui::HierarchyWidget      _hierarchyWidget;   /** Widget for displaying the actions */
};