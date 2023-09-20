// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include <CoreInterface.h>
#include <AbstractTaskManager.h>

#include <actions/TaskAction.h>
#include <actions/ProgressAction.h>
#include <actions/TriggerAction.h>

#include <util/Icon.h>

#include <QStyledItemDelegate>
#include <QStyleOptionButton>
#include <QHeaderView>
#include <QPainter>
#include <QSortFilterProxyModel>

namespace hdps::gui {

const QSize TasksAction::tasksIconPixmapSize = QSize(64, 64);

TasksAction::TasksAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _tasksModel(this),
    _tasksFilterModel(this),
    _tasksIconPixmap(),
    _rowHeight(20),
    _progressColumnMargin(0),
    _autoHideKillCollumn(true)
{
    _tasksFilterModel.setSourceModel(&_tasksModel);
    _tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "RunningIndeterminate", "Finished" });

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

void TasksAction::setRowHeight(std::int32_t rowHeight)
{
    _rowHeight = rowHeight;
}

std::int32_t TasksAction::getRowHeight() const
{
    return _rowHeight;
}

void TasksAction::setProgressColumnMargin(std::int32_t progressColumnMargin)
{
    _progressColumnMargin = progressColumnMargin;
}

std::int32_t TasksAction::getProgressColumnMargin() const
{
    return _progressColumnMargin;
}

void TasksAction::setAutoHideKillCollumn(bool autoHideKillColumn)
{
    if (autoHideKillColumn == _autoHideKillCollumn)
        return;

    _autoHideKillCollumn = autoHideKillColumn;

    emit autoHideKillCollumnChanged(_autoHideKillCollumn);
}

bool TasksAction::getAutoHideKillCollumn() const
{
    return _autoHideKillCollumn;
}

void TasksAction::openPersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent /*= QModelIndex()*/)
{
    for (int rowIndex = 0; rowIndex < _tasksFilterModel.rowCount(parent); ++rowIndex) {
        const auto index = _tasksFilterModel.index(rowIndex, 0, parent);

        const auto persistentEditorsIndexes = QModelIndexList({
            index.siblingAtColumn(static_cast<int>(TasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(TasksModel::Column::Kill))
        });

        for (const auto& persistentEditorIndex : persistentEditorsIndexes)
            if (!itemView.isPersistentEditorOpen(persistentEditorIndex))
                itemView.openPersistentEditor(persistentEditorIndex);

        if (_tasksFilterModel.hasChildren(index))
            openPersistentProgressEditorsRecursively(itemView, index);
    }
}

void TasksAction::closePersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent /*= QModelIndex()*/)
{
    for (int rowIndex = 0; rowIndex < _tasksFilterModel.rowCount(parent); ++rowIndex) {
        const auto index = _tasksFilterModel.index(rowIndex, 0, parent);

        const auto persistentEditorsIndexes = QModelIndexList({
            index.siblingAtColumn(static_cast<int>(TasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(TasksModel::Column::Kill))
        });

        for (const auto& persistentEditorIndex : persistentEditorsIndexes)
            if (itemView.isPersistentEditorOpen(persistentEditorIndex))
                itemView.closePersistentEditor(persistentEditorIndex);

        if (_tasksFilterModel.hasChildren(index))
            closePersistentProgressEditorsRecursively(itemView, index);
    }
}

bool TasksAction::hasAgregateTasks() const
{
    const auto matches = _tasksFilterModel.match(_tasksFilterModel.index(0, static_cast<int>(TasksModel::Column::ProgressMode)), Qt::EditRole, static_cast<int>(Task::ProgressMode::Aggregate), -1, Qt::MatchExactly | Qt::MatchRecursive);

    return !matches.isEmpty();
}

/** Tree view item delegate class for showing custom task progress user interface */
class ProgressItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit ProgressItemDelegate(TasksAction* tasksAction) :
        QStyledItemDelegate(tasksAction),
        _tasksAction(tasksAction)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        auto progressAction = getProgressAction(index);

        if (progressAction == nullptr)
            return nullptr;

        return progressAction->createWidget(parent);
    }
    
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const {
        editor->setGeometry(option.rect);
    }

private:

    /**
     * Get progress action for model \p index
     * @param index Model index to retrieve the progress action for
     * @return Pointer to progress action
     */
    ProgressAction* getProgressAction(const QModelIndex& index) const {
        auto item = _tasksAction->getTasksModel().itemFromIndex(_tasksAction->getTasksFilterModel().mapToSource(index).siblingAtColumn(static_cast<int>(TasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<TasksModel::ProgressItem*>(item)->getTaskAction().getProgressAction());
    }

private:
    TasksAction*    _tasksAction;
};

/** Tree view item delegate class for showing custom task kill user interface */
class KillTaskItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    explicit KillTaskItemDelegate(TasksAction* tasksAction) :
        QStyledItemDelegate(tasksAction),
        _tasksAction(tasksAction)
    {
    }

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
        auto killTaskAction = getKillTaskAction(index);

        if (killTaskAction == nullptr)
            return nullptr;

        return killTaskAction->createWidget(parent);
    }

    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&/*index*/) const {
        editor->setGeometry(option.rect);
    }

private:

    /**
     * Get kill task action for model \p index
     * @param index Model index to retrieve the task killer action for
     * @return Pointer to task killer action
     */
    TriggerAction* getKillTaskAction(const QModelIndex& index) const {
        auto item = _tasksAction->getTasksModel().itemFromIndex(_tasksAction->getTasksFilterModel().mapToSource(index).siblingAtColumn(static_cast<int>(TasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<TasksModel::ProgressItem*>(item)->getTaskAction().getKillTaskAction());
    }

private:
    TasksAction* _tasksAction;
};

TasksAction::Widget::Widget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksAction, widgetFlags),
    _tasksAction(tasksAction),
    _tasksWidget(this, "Task", tasksAction->getTasksModel(), &tasksAction->getTasksFilterModel(), widgetFlags & Toolbar, widgetFlags & Overlay)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check"));

    _tasksWidget.setObjectName("HierarchyWidget");
    _tasksWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskTypeFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskScopeFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskStatusFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getHideDisabledTasksFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getHideHiddenTasksFilterAction());
    _tasksWidget.setHeaderHidden(false);

    _tasksWidget.getFilterColumnAction().setCurrentText("Name");

    auto& treeView = _tasksWidget.getTreeView();

    treeView.setObjectName("TreeView");
    treeView.setRootIsDecorated(true);

    treeView.setItemDelegateForColumn(static_cast<int>(TasksModel::Column::Progress), new ProgressItemDelegate(tasksAction));
    treeView.setItemDelegateForColumn(static_cast<int>(TasksModel::Column::Kill), new KillTaskItemDelegate(tasksAction));

    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Enabled), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Visible), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressDescription), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressText), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ProgressMode), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ParentID), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Scope), true);
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::MayKill), true);

    auto treeViewHeader = treeView.header();

    treeView.setTextElideMode(Qt::ElideMiddle);
    treeView.setItemsExpandable(true);
    
    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::ExpandCollapse), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Status), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Name), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Enabled), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Visible), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Progress), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::ProgressDescription), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::ProgressText), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::ID), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::ParentID), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Type), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::MayKill), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(TasksModel::Column::Kill), QHeaderView::ResizeToContents);

    connect(&tasksAction->getTasksFilterModel(), &QSortFilterProxyModel::layoutChanged, this, [this]() -> void {
        _tasksAction->openPersistentProgressEditorsRecursively(_tasksWidget.getTreeView());

        QCoreApplication::processEvents();
    });

    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto index = _tasksAction->getTasksFilterModel().index(rowIndex, 0, parent);

            const auto persistentEditorsIndexes = QModelIndexList({
                index.siblingAtColumn(static_cast<int>(TasksModel::Column::Progress)),
                index.siblingAtColumn(static_cast<int>(TasksModel::Column::Kill))
                });

            for (const auto& persistentEditorIndex : persistentEditorsIndexes)
                if (_tasksWidget.getTreeView().isPersistentEditorOpen(persistentEditorIndex))
                    _tasksWidget.getTreeView().closePersistentEditor(persistentEditorIndex);
        }
    });

    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::layoutChanged, this, &TasksAction::Widget::updateTreeView);

    updateTreeView();

    auto layout = new QVBoxLayout();

    layout->addWidget(&_tasksWidget);

    setLayout(layout);

    /*
    connect(&treeView, &QTreeView::clicked, this, [this](const QModelIndex& index) -> void {
        if (static_cast<TasksModel::Column>(index.column()) != TasksModel::Column::Kill)
            return;

        const auto selectedItemIndex    = _tasksAction->getTasksFilterModel().mapToSource(index);
        const auto taskItem             = static_cast<TasksModel::Item*>(_tasksAction->getTasksModel().itemFromIndex(selectedItemIndex));

        taskItem->getTask()->kill();
    });

    */

    _tasksAction->openPersistentProgressEditorsRecursively(treeView);
}

void TasksAction::Widget::updateTreeView()
{
    auto& treeView = _tasksWidget.getTreeView();

    treeView.setRootIsDecorated(_tasksAction->hasAgregateTasks());
    treeView.setColumnHidden(static_cast<int>(TasksModel::Column::ExpandCollapse), !_tasksAction->hasAgregateTasks());

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

    if (_tasksAction->getAutoHideKillCollumn())
        treeView.setColumnHidden(static_cast<int>(TasksModel::Column::Kill), _tasksAction->getTasksFilterModel().match(_tasksAction->getTasksFilterModel().index(0, static_cast<int>(TasksModel::Column::MayKill)), Qt::EditRole, true).isEmpty());
}

}
