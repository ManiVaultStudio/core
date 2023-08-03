// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include <CoreInterface.h>
#include <AbstractTaskManager.h>

#include <actions/TaskAction.h>
#include <actions/ProgressAction.h>

#include <util/Icon.h>

#include <QStyledItemDelegate>
#include <QHeaderView>
#include <QPainter>

namespace hdps::gui {

const QSize TasksAction::tasksIconPixmapSize = QSize(64, 64);

TasksAction::TasksAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _tasksModel(this),
    _tasksFilterModel(this),
    _tasksIconPixmap()
{
    _tasksFilterModel.setSourceModel(&_tasksModel);
    _tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "RunningIndeterminate" });

    _tasksIconPixmap = Application::getIconFont("FontAwesome").getIcon("tasks").pixmap(tasksIconPixmapSize);

    connect(&_tasksFilterModel, &QAbstractItemModel::layoutChanged, this, &TasksAction::filterModelChanged);
}

TasksModel& TasksAction::getTasksModel()
{
    return _tasksModel;
}

TasksFilterModel& TasksAction::getTasksFilterModel()
{
    return _tasksFilterModel;
}

void TasksAction::filterModelChanged()
{
    QPixmap iconPixmap(tasksIconPixmapSize);

    iconPixmap.fill(Qt::transparent);

    QPainter painter(&iconPixmap);

    const auto scaledTasksIconPixmapSize = tasksIconPixmapSize - 0.25 * tasksIconPixmapSize;

    painter.drawPixmap(QPoint(0, 0), _tasksIconPixmap.scaled(scaledTasksIconPixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const auto numberOfTasks = _tasksFilterModel.rowCount();

    const auto badgeRadius = 43.0;

    painter.setPen(QPen(QColor(numberOfTasks == 0 ? 0 : 255, 0, 0, 255), badgeRadius, Qt::SolidLine, Qt::RoundCap));

    const auto center = QPoint(tasksIconPixmapSize.width() - (badgeRadius / 2), tasksIconPixmapSize.height() - (badgeRadius / 2));

    painter.drawPoint(center);

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", numberOfTasks >= 10 ? 18 : 24, 900));

    const auto textRectangleSize = QSize(32, 32);
    const auto textRectangle = QRectF(center - QPointF(textRectangleSize.width() / 2.f, textRectangleSize.height() / 2.f), textRectangleSize);

    painter.drawText(textRectangle, QString::number(numberOfTasks), QTextOption(Qt::AlignCenter));

    setIcon(createIcon(iconPixmap));
    setToolTip(QString("Tasks: %1").arg(QString::number(numberOfTasks)));
}

/** Tree view item delegate class for showing custom task progress user interface */
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
        progressAction.setTextFormat(status == Task::Status::Running || status == Task::Status::RunningIndeterminate ? QString("%1 %p%").arg(progressDescription) : progressDescription);
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
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::MayKill), true);
    //treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Kill), true);

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

    connect(&treeView, &QTreeView::customContextMenuRequested, [this, tasksAction](const QPoint& point) {
        const auto selectedRows = _tasksWidget.getTreeView().selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        auto mayDestroyTasks = false;

        QList<Task*> tasks, killableTasks;

        for (const auto& selectedRow : selectedRows) {
            const auto selectedItemIndex    = tasksAction->getTasksFilterModel().mapToSource(selectedRow);
            const auto taskItem             = static_cast<TasksModel::Item*>(tasksAction->getTasksModel().itemFromIndex(selectedItemIndex));

            tasks << taskItem->getTask();
        }
            
        for (auto task : tasks) {
            if (!task->getMayKill())
                continue;

            killableTasks << task;

            if (!mayDestroyTasks)
                mayDestroyTasks = true;
        }

        if (!mayDestroyTasks)
            return;

        QMenu contextMenu;

        const auto actionName = QString("Kill %1 task%2").arg(QString::number(selectedRows.count()), selectedRows.count() >= 2 ? "s" : "");

        contextMenu.addAction(Application::getIconFont("FontAwesome").getIcon("trash"), actionName, [killableTasks] {
            for (auto killableTask : killableTasks)
                killableTask->kill();
        });

        contextMenu.exec(QCursor::pos());
    });
}

void TasksAction::Widget::modelChanged()
{
    const auto rowCount = _tasksWidget.getModel().rowCount();

    setEnabled(rowCount > 0);

    if (rowCount <= 0)
        return;

    auto treeViewHeader = _tasksWidget.getTreeView().header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);
}

}
