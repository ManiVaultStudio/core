#pragma once

#include <models/ActionsFilterModel.h>
#include <widgets/HierarchyWidget.h>

#include <QWidget>

namespace hdps::gui
{

/**
 * Actions widget class
 *
 * Widget class for viewing the actions model from the core actions manager
 *
 * @author Thomas Kroes
 */
class ActionsWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param rootAction Pointer to non-owning root action
     */
    ActionsWidget(QWidget* parent, WidgetAction* rootAction);

private:

    /**
     * Set action highlight state
     * @param index Model index of the action to highlight
     */
    void setActionHighlighted(const QModelIndex& index, bool highlighted);
    
    /**
     * Leave event invoked when the mouse pointer leaves the widget
     * @param event Pointer to event
     */
    void leaveEvent(QEvent* event) override;

private:
    ActionsFilterModel      _filterModel;               /** Hierarchical actions filter model */
    HierarchyWidget         _hierarchyWidget;           /** Widget for displaying action hierarchy */
    QPersistentModelIndex   _lastHoverModelIndex;       /** Model index of the item that was last hovered */
};

}
