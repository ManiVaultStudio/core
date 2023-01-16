#include "StartPageActionsWidget.h"
#include "StartPageActionDelegate.h"
#include "StartPageWidget.h"

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
    treeView.setItemDelegateForColumn(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), new StartPageActionDelegate());
    treeView.setItemDelegateForColumn(static_cast<int>(StartPageActionsModel::Column::CommentsDelegate), new StartPageActionDelegate());
    treeView.setSelectionBehavior(QAbstractItemView::SelectRows);
    treeView.setSelectionMode(QAbstractItemView::SingleSelection);
    treeView.setIconSize(QSize(24, 24));
    treeView.setStyleSheet(" \
        QTreeView { \
            border: none; \
        } \
    ");

    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Title), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Description), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Comments), true);
    treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::Callback), true);

    QPalette treeViewPalette(treeView.palette());

    QStyleOption styleOption;

    styleOption.initFrom(&treeView);

    treeViewPalette.setColor(QPalette::Base, styleOption.palette.color(QPalette::Normal, QPalette::Midlight));

    treeView.setPalette(treeViewPalette);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(StartPageActionsModel::Column::Icon), QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(static_cast<int>(StartPageActionsModel::Column::SummaryDelegate), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(StartPageActionsModel::Column::CommentsDelegate), QHeaderView::ResizeToContents);

    treeViewHeader->resizeSection(static_cast<int>(StartPageActionsModel::Column::Icon), 34);

    connect(&_model, &QStandardItemModel::layoutChanged, this, [this, &treeView]() -> void {
        treeView.setColumnHidden(static_cast<int>(StartPageActionsModel::Column::CommentsDelegate), !_model.hasComments());
        treeView.resizeColumnToContents(static_cast<int>(StartPageActionsModel::Column::CommentsDelegate));
    });

    connect(&treeView, &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        auto callback = index.siblingAtColumn(static_cast<int>(StartPageActionsModel::Column::Callback)).data(Qt::UserRole + 1).value<StartPageActionsModel::ClickedCB>();
        callback();

        _hierarchyWidget.getSelectionModel().clear();
    });
}

StartPageActionsModel& StartPageActionsWidget::getModel()
{
    return _model;
}

HierarchyWidget& StartPageActionsWidget::getHierarchyWidget()
{
    return _hierarchyWidget;
}
