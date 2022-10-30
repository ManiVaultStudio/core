#pragma once

#include "HierarchyWidget.h"
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
     */
    ActionHierarchyWidget(QWidget* parent);

private:
    ActionHierarchyModel        _model;             /** Hierarchical action model */
    ActionHierarchyFilterModel  _filterModel;       /** Hierarchical action filter model */
    HierarchyWidget             _hierarchyWidget;   /** Widget for displaying hierarchy */
};

}
}
