#pragma once

#include "models/AbstractActionsModel.h"
#include "models/ActionsFilterModel.h"
#include "widgets/HierarchyWidget.h"

#include <QWidget>

namespace hdps::gui
{

/**
 * Actions widget class
 *
 * Widget class for viewing the contents of an actions model (e.g. list or hierarchy)
 *
 * @author Thomas Kroes
 */
class ActionsWidget final : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param actionsModel Reference to actions input model
     */
    ActionsWidget(QWidget* parent, AbstractActionsModel& actionsModel);

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    HierarchyWidget& getHierarchyWidget();

private:
    
    /**
     * Leave event invoked when the mouse pointer leaves the widget
     * @param event Pointer to event
     */
    void leaveEvent(QEvent* event) override;

    /** Resize sections based on content */
    void resizeSectionsToContent();

private:
    AbstractActionsModel&   _actionsModel;              /** Input actions model */
    ActionsFilterModel      _filterModel;               /** Hierarchical actions filter model */
    HierarchyWidget         _hierarchyWidget;           /** Widget for displaying action hierarchy */
    QPersistentModelIndex   _lastHoverModelIndex;       /** Model index of the item that was last hovered */
};

}
