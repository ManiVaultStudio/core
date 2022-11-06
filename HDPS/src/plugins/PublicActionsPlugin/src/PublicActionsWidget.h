#pragma once

#include <widgets/HierarchyWidget.h>

#include <ActionsFilterModel.h>

#include <QWidget>

using namespace hdps;
using namespace hdps::gui;

/**
 * Public actions widget class
 *
 * Widget class for viewing/editing public actions.
 *
 * @author Thomas Kroes
 */
class PublicActionsWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    PublicActionsWidget(QWidget* parent);

private:
    ActionsFilterModel      _filterModel;           /** Actions filter model */
    HierarchyWidget         _hierarchyWidget;       /** Hierarchy widget */
};
