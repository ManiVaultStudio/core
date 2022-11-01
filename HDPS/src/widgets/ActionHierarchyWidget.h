#pragma once

#include "HierarchyWidget.h"
#include "ActionHierarchyModel.h"
#include "ActionHierarchyFilterModel.h"

#include "actions/ToggleAction.h"

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

    /** Destructor */
    ~ActionHierarchyWidget();

private:

    /**
     * Set action highlight state
     * @param index Model index of the action to highlight
     */
    void setActionHighlighted(const QModelIndex& index, bool highlighted);

private:
    ActionHierarchyModel        _model;                             /** Hierarchical action model */
    ActionHierarchyFilterModel  _filterModel;                       /** Hierarchical action filter model */
    HierarchyWidget             _hierarchyWidget;                   /** Widget for displaying hierarchy */
    QModelIndex                 _lastHoverModelIndex;               /** Model index of the item that was last hovered */
    ToggleAction                _filterVisibleAction;               /** Filter visible action */
    ToggleAction                _filterNotVisibleAction;            /** Filter not visible action */
    ToggleAction                _filterMayPublishAction;            /** Filter may publish action */
    ToggleAction                _filterMayNotPublishAction;         /** Filter may not publish action */
    ToggleAction                _filterMayConnectAction;            /** Filter may connect action */
    ToggleAction                _filterMayNotConnectAction;         /** Filter may not connect action */
    ToggleAction                _filterMayDisconnectAction;         /** Filter may disconnect action */
    ToggleAction                _filterMayNotDisconnectAction;      /** Filter may not disconnect action */
};

}
}
