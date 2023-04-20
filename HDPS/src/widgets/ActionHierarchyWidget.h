#pragma once

#include <models/ActionHierarchyModel.h>
#include <models/ActionHierarchyFilterModel.h>
#include <widgets/HierarchyWidget.h>

#include <QWidget>
#include <QTreeView>

namespace hdps
{

namespace gui
{

/**
 * Action hierarchy widget class
 *
 * Widget class for configuring an action hierarchy
 *
 * @author Thomas Kroes
 */
class ActionHierarchyWidget : public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param rootAction Pointer to non-owning root action
     */
    ActionHierarchyWidget(QWidget* parent, WidgetAction* rootAction);

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
    ActionHierarchyModel        _model;                     /** Hierarchical action model */
    ActionHierarchyFilterModel  _filterModel;               /** Hierarchical action filter model */
    HierarchyWidget             _hierarchyWidget;           /** Widget for displaying hierarchy */
    QPersistentModelIndex       _lastHoverModelIndex;       /** Model index of the item that was last hovered */
};

}
}
