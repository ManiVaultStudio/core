// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksAction.h"

#include "CoreInterface.h"
#include "AbstractTaskManager.h"

#include "actions/TaskAction.h"
#include "actions/ProgressAction.h"
#include "actions/TriggerAction.h"

#include "models/TasksTreeModel.h"

#include "util/Icon.h"

#include <QStyledItemDelegate>
#include <QStyleOptionButton>
#include <QHeaderView>
#include <QPainter>
#include <QSortFilterProxyModel>

namespace mv::gui {

const QSize TasksAction::tasksIconPixmapSize = QSize(64, 64);

TasksAction::TasksAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title),
    _tasksFilterModel(this),
    _tasksIconPixmap(),
    _rowHeight(20),
    _progressColumnMargin(0),
    _autoHideKillCollumn(true)
{
    setDefaultWidgetFlags(WidgetFlag::Tree);
    setPopupSizeHint(QSize(600, 0));

    _tasksFilterModel.setSourceModel(tasks().getTreeModel());
    _tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
}

TasksFilterModel& TasksAction::getTasksFilterModel()
{
    return _tasksFilterModel;
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
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
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
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
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
    const auto matches = _tasksFilterModel.match(_tasksFilterModel.index(0, static_cast<int>(AbstractTasksModel::Column::ProgressMode)), Qt::EditRole, static_cast<int>(Task::ProgressMode::Aggregate), -1, Qt::MatchExactly | Qt::MatchRecursive);

    return !matches.isEmpty();
}

QWidget* TasksAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this, widgetFlags);
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    if (widgetFlags & WidgetFlag::Tree)
        layout->addWidget(new TasksAction::TreeWidget(parent, this, widgetFlags));

    if (widgetFlags & WidgetFlag::Popup)
        layout->addWidget(new TasksAction::PopupWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
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
        auto item = tasks().getTreeModel()->itemFromIndex(_tasksAction->getTasksFilterModel().mapToSource(index).siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<AbstractTasksModel::ProgressItem*>(item)->getTaskAction().getProgressAction());
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
        auto item = tasks().getTreeModel()->itemFromIndex(_tasksAction->getTasksFilterModel().mapToSource(index).siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<AbstractTasksModel::ProgressItem*>(item)->getTaskAction().getKillTaskAction());
    }

private:
    TasksAction* _tasksAction;
};

TasksAction::TreeWidget::TreeWidget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksAction, widgetFlags),
    _tasksAction(tasksAction),
    _tasksWidget(this, "Task", *tasks().getTreeModel(), &tasksAction->getTasksFilterModel(), widgetFlags)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("check"));

    _tasksWidget.setObjectName("HierarchyWidget");
    _tasksWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskTypeFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskScopeFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTaskStatusFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getTopLevelTasksOnlyAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getHideDisabledTasksFilterAction());
    _tasksWidget.getFilterGroupAction().addAction(&tasksAction->getTasksFilterModel().getHideHiddenTasksFilterAction());
    _tasksWidget.setHeaderHidden(false);

    _tasksWidget.getFilterColumnAction().setCurrentText("Name");

    auto& treeView = _tasksWidget.getTreeView();

    treeView.setObjectName("TreeView");
    treeView.setRootIsDecorated(true);

    treeView.setItemDelegateForColumn(static_cast<int>(AbstractTasksModel::Column::Progress), new ProgressItemDelegate(tasksAction));
    treeView.setItemDelegateForColumn(static_cast<int>(AbstractTasksModel::Column::Kill), new KillTaskItemDelegate(tasksAction));

    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Enabled), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Visible), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ProgressDescription), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ProgressText), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ProgressMode), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ID), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ParentID), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::GuiScopes), true);
    treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::MayKill), true);

    auto treeViewHeader = treeView.header();

    treeView.setTextElideMode(Qt::ElideMiddle);
    treeView.setItemsExpandable(true);
    
    treeViewHeader->setStretchLastSection(false);

    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Name), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Enabled), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Visible), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Progress), QHeaderView::Stretch);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::ProgressDescription), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::ProgressText), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Status), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::ID), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::ParentID), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Type), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::MayKill), QHeaderView::ResizeToContents);
    treeViewHeader->setSectionResizeMode(static_cast<int>(AbstractTasksModel::Column::Kill), QHeaderView::ResizeToContents);

    connect(&tasksAction->getTasksFilterModel(), &QSortFilterProxyModel::rowsInserted, this, [this]() -> void {
        _tasksAction->openPersistentProgressEditorsRecursively(_tasksWidget.getTreeView());

        QCoreApplication::processEvents();
    });

    connect(&tasksAction->getTasksFilterModel(), &QSortFilterProxyModel::rowsAboutToBeRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
        for (int rowIndex = first; rowIndex <= last; rowIndex++) {
            const auto index = _tasksAction->getTasksFilterModel().index(rowIndex, 0, parent);

            const auto persistentEditorsIndexes = QModelIndexList({
                index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
                index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
            });

            for (const auto& persistentEditorIndex : persistentEditorsIndexes)
                if (_tasksWidget.getTreeView().isPersistentEditorOpen(persistentEditorIndex))
                    _tasksWidget.getTreeView().closePersistentEditor(persistentEditorIndex);
        }
    });

    connect(&tasksAction->getTasksFilterModel(), &QAbstractItemModel::layoutChanged, this, &TasksAction::TreeWidget::updateTreeView);

    updateTreeView();

    auto layout = new QVBoxLayout();

    layout->addWidget(&_tasksWidget);

    setLayout(layout);

    _tasksAction->openPersistentProgressEditorsRecursively(treeView);
}

void TasksAction::TreeWidget::updateTreeView()
{
    auto& treeView = _tasksWidget.getTreeView();

    //treeView.setRootIsDecorated(_tasksAction->hasAgregateTasks());
    //treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::ExpandCollapse), !_tasksAction->hasAgregateTasks());

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSections(QHeaderView::ResizeMode::ResizeToContents);

    if (_tasksAction->getAutoHideKillCollumn())
        treeView.setColumnHidden(static_cast<int>(AbstractTasksModel::Column::Kill), _tasksAction->getTasksFilterModel().match(_tasksAction->getTasksFilterModel().index(0, static_cast<int>(AbstractTasksModel::Column::MayKill)), Qt::EditRole, true).isEmpty());
}

TasksAction::PopupWidget::PopupWidget(QWidget* parent, TasksAction* tasksAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, tasksAction, widgetFlags),
    _tasksAction(tasksAction)
{
    setLayout(new QGridLayout());

    auto& tasksFilterModel = _tasksAction->getTasksFilterModel();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, &PopupWidget::updateLayout);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, &PopupWidget::updateLayout);

    updateLayout();
}

void TasksAction::PopupWidget::cleanLayout()
{
    QLayoutItem* item;

    while ((item = this->layout()->takeAt(0)) != 0)
        delete item;
}

void TasksAction::PopupWidget::updateLayout()
{
    auto& tasksFilterModel  = _tasksAction->getTasksFilterModel();
    auto& tasksModel        = *dynamic_cast<QStandardItemModel*>(tasksFilterModel.sourceModel());

    const auto numberOfTasks = tasksFilterModel.rowCount();

    cleanLayout();

    QVector<Task*> currentTasks;

    auto gridLayout = static_cast<QGridLayout*>(layout());

    gridLayout->setColumnStretch(1, 1);

    for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex) {
        const auto nameSourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)));
        const auto progressSourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (!nameSourceModelIndex.isValid() || !progressSourceModelIndex.isValid())
            continue;

        auto nameItem = dynamic_cast<AbstractTasksModel::NameItem*>(tasksModel.itemFromIndex(nameSourceModelIndex));
        auto progressItem = dynamic_cast<AbstractTasksModel::ProgressItem*>(tasksModel.itemFromIndex(progressSourceModelIndex));

        Q_ASSERT(nameItem != nullptr);
        Q_ASSERT(progressItem != nullptr);

        if (nameItem == nullptr || progressItem == nullptr)
            continue;

        currentTasks << progressItem->getTask();

        if (!_widgetsMap.contains(progressItem->getTask())) {
            auto labelWidget = nameItem->getStringAction().createWidget(this, StringAction::Label);
            auto progressWidget = progressItem->getTaskAction().createWidget(this);

            progressWidget->setFixedHeight(18);

            _widgetsMap[progressItem->getTask()] = { labelWidget, progressWidget };
        }

        const auto rowCount = gridLayout->rowCount();

        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][0], rowCount, 0);
        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][1], rowCount, 1);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        for (auto widget : _widgetsMap[task])
            delete widget;

        _widgetsMap.remove(task);
    }
}

}
