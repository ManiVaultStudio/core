// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include <CoreInterface.h>
#include <AbstractTaskManager.h>

#include <actions/TaskAction.h>

#include <QStyledItemDelegate>
#include <QHeaderView>

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
        QStyleOptionProgressBar progressBarStyleOption;

        progressBarStyleOption.rect = option.rect;

        const auto status               = static_cast<Task::Status>(index.siblingAtColumn(static_cast<int>(TasksModel::Column::Status)).data(Qt::EditRole).toInt());
        const auto progress             = static_cast<int>(index.data(Qt::EditRole).toFloat() * 100.f);
        const auto progressDescription  = index.siblingAtColumn(static_cast<int>(TasksModel::Column::ProgressDescription)).data(Qt::EditRole).toString();
        const auto progressText         = index.siblingAtColumn(static_cast<int>(TasksModel::Column::ProgressText)).data(Qt::EditRole).toString();
        const auto text                 = QString(" %1 ").arg(progressText);

        progressBarStyleOption.minimum          = 0;
        progressBarStyleOption.maximum          = status == Task::Status::RunningIndeterminate ? 0 : 100;
        progressBarStyleOption.textAlignment    = Qt::AlignCenter;
        progressBarStyleOption.progress         = progress;
        progressBarStyleOption.text             = progressBarStyleOption.fontMetrics.elidedText(text, Qt::ElideMiddle, progressBarStyleOption.rect.width());
        progressBarStyleOption.textVisible      = true;

        qApp->style()->drawControl(QStyle::CE_ProgressBar, &progressBarStyleOption, painter);
    }
};

TasksAction::Widget::Widget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksAction, widgetFlags),
    _tasksWidget(this, "Task", tasksAction->getTasksModel(), &tasksAction->getTasksFilterModel(), widgetFlags & Toolbar, widgetFlags & Overlay)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check"));

    _tasksWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check"));

    auto& treeView = _tasksWidget.getTreeView();

    treeView.setRootIsDecorated(false);

    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ParentID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressDescription), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressText), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);

    treeView.setItemDelegateForColumn(static_cast<int>(TasksModel::Column::Progress), new ProgressItemDelegate(this));

    auto treeViewHeader = treeView.header();

    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Name), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Progress), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Status), QHeaderView::Fixed);

    treeViewHeader->resizeSection(static_cast<int>(TasksModel::Column::Status), 130);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_tasksWidget);

    setLayout(layout);
}

}
