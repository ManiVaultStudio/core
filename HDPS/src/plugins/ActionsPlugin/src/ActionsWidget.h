#pragma once

#include <widgets/HierarchyWidget.h>

#include <models/ActionsModel.h>
#include <models/ActionsFilterModel.h>

#include <QWidget>

using namespace hdps;
using namespace hdps::gui;

/**
 * Actions widget class
 *
 * Widget class for viewing/editing actions.
 *
 * @author Thomas Kroes
 */
class ActionsWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent
     * @param parent Pointer to parent widget
     */
    ActionsWidget(QWidget* parent = nullptr);

private:
    ActionsModel            _model;                 /** Actions model */
    ActionsFilterModel      _filterModel;           /** Actions filter model for actions model */
    HierarchyWidget         _hierarchyWidget;       /** Hierarchy widget */
};
