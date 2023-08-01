// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include <CoreInterface.h>
#include <AbstractTaskManager.h>

#include <actions/TaskAction.h>
#include <actions/ProgressAction.h>

#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QPainter>

namespace hdps::gui {

TasksAction::TasksAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _tasksModel(this),
    _tasksFilterModel(this)
{
    setConnectionPermissionsToForceNone();
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    setIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
    setDefaultWidgetFlag(GroupAction::Vertical);

    _tasksFilterModel.setSourceModel(&_tasksModel);
    //_tasksFilterModel.setFilterKeyColumn(static_cast<int>(TasksModel::Column::Name));
}

TasksModel& TasksAction::getTasksModel()
{
    return _tasksModel;
}

TasksFilterModel& TasksAction::getTasksFilterModel()
{
    return _tasksFilterModel;
}

/** Tree view item delegate class for overriding painting of toggle columns */
class ProgressItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit ProgressItemDelegate(QObject* parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

protected:

    /** Paints a progress bar in the cell */
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override
    {
        const auto status               = static_cast<Task::Status>(index.siblingAtColumn(static_cast<int>(TasksModel::Column::Status)).data(Qt::EditRole).toInt());
        const auto progress             = static_cast<int>(index.data(Qt::EditRole).toFloat() * 100.f);
        const auto progressDescription  = index.siblingAtColumn(static_cast<int>(TasksModel::Column::ProgressDescription)).data(Qt::EditRole).toString();

        ProgressAction progressAction(nullptr, "Progress");

        progressAction.setRange(0, status == Task::Status::RunningIndeterminate ? 0 : 100);
        progressAction.setTextFormat(QString("%1 %p%").arg(progressDescription));
        progressAction.setProgress(progress);

        auto progressWidget = ProgressAction::BarWidget(nullptr, &progressAction, 0);

        progressWidget.setGeometry(option.rect);

        painter->save();
        painter->translate(option.rect.topLeft());

        progressWidget.render(painter);
        
        painter->restore();
    }
};

TasksAction::Widget::Widget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksAction, widgetFlags),
    _tasksWidget(this, "Task", tasksAction->getTasksModel(), &tasksAction->getTasksFilterModel(), widgetFlags & Toolbar, widgetFlags & Overlay)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check"));

    _tasksWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
    //_tasksWidget.setHeaderHidden(true);
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskTypeFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskStatusFilterAction());

    _tasksWidget.getFilterColumnAction().setCurrentText("Name");

    //_tasksWidget.getToolbarAction().addAction(&tasksAction->getTasksFilterModel().getTaskStatusFilterAction());

    auto& treeView = _tasksWidget.getTreeView();

    treeView.setRootIsDecorated(false);

    //treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Status), true);
    //treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Name), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ParentID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressDescription), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressText), true);
    //treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Progress), QHeaderView::Stretch);

    treeView.setTextElideMode(Qt::ElideMiddle);
    treeView.setItemDelegateForColumn(static_cast<int>(TasksModel::Column::Progress), new ProgressItemDelegate(this));

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Progress), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Status), QHeaderView::Fixed);

    treeViewHeader->resizeSection(static_cast<int>(TasksModel::Column::Status), 130);

    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::rowsInserted, this, &TasksAction::Widget::modelChanged);
    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::rowsRemoved, this, &TasksAction::Widget::modelChanged);
    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::layoutChanged, this, &TasksAction::Widget::modelChanged);

    connect(&treeView, &HierarchyWidgetTreeView::columnHiddenChanged, this, &TasksAction::Widget::modelChanged);
    connect(&treeView, &HierarchyWidgetTreeView::expanded, this, &TasksAction::Widget::modelChanged);
    connect(&treeView, &HierarchyWidgetTreeView::collapsed, this, &TasksAction::Widget::modelChanged);

    modelChanged();

    auto layout = new QVBoxLayout();

    layout->addWidget(&_tasksWidget);

    setLayout(layout);
}

void TasksAction::Widget::modelChanged()
{
    if (_tasksWidget.getModel().rowCount() <= 0)
        return;

    auto treeViewHeader = _tasksWidget.getTreeView().header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

}
