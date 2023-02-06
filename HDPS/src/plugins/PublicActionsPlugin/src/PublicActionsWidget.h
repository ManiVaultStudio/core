#pragma once

#include "PublicActionsModel.h"
#include "PublicActionsFilterModel.h"

#include <widgets/HierarchyWidget.h>

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
     * Construct with \p parent
     * @param parent Pointer to parent widget
     */
    PublicActionsWidget(QWidget* parent = nullptr);

private:
    PublicActionsModel          _model;             /** Public actions model */
    PublicActionsFilterModel    _filterModel;       /** Public actions filter model for public actions model */
    HierarchyWidget             _hierarchyWidget;   /** Hierarchy widget for displaying the public actions */
};
