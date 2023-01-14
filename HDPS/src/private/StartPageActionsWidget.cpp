#include "StartPageActionsWidget.h"
#include "StartPageActionDelegate.h"

#include <Application.h>

#include <QDebug>
#include <QHeaderView>

using namespace hdps;
using namespace hdps::gui;

StartPageActionsWidget::StartPageActionsWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Item", _model, &_filterModel, true, false)    
{
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _filterModel.setFilterKeyColumn(static_cast<int>(StartPageActionsModel::Column::Title));

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _hierarchyWidget.setWindowIcon(fontAwesome.getIcon("play"));

    _hierarchyWidget.getFilterGroupAction().setVisible(false);
    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    _hierarchyWidget.getColumnsGroupAction().setVisible(false);
    _hierarchyWidget.getSelectionGroupAction().setVisible(false);
    _hierarchyWidget.setHeaderHidden(true);

    auto& treeView = _hierarchyWidget.getTreeView();

    treeView.setRootIsDecorated(false);
    treeView.setItemDelegateForColumn(static_cast<int>(StartPageActionsModel::Column::Summary), new StartPageActionDelegate());
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setStyleSheet("border: none;");

    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Description), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSection(static_cast<int>(StartPageActionsModel::Column::Icon), 32);
}

StartPageActionsModel& StartPageActionsWidget::getModel()
{
    return _model;
}

HierarchyWidget& StartPageActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}
