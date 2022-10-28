#pragma once

#include "ActionHierarchyModel.h"
#include "ActionHierarchyFilterModel.h"

#include <QWidget>
#include <QTreeView>

namespace hdps
{

namespace gui
{

/**
 * Action hierarchy widget class
 *
 * Widget class or interaction with an action hierarchy
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
     */
    ActionHierarchyWidget(QWidget* parent);

private:
    ActionHierarchyModel        _model;             /** Model containing actions to be displayed in the hierarchy */
    ActionHierarchyFilterModel  _filterModel;       /** Action hierarchy filter model */
    QTreeView                   _treeView;          /** Tree view that contains the action hierarchy */
    QItemSelectionModel         _selectionModel;    /** Selection model */
};

}
}
