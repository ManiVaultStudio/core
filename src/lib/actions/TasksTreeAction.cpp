// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksTreeAction.h"

#include "CoreInterface.h"

#include "actions/TaskAction.h"
#include "actions/ProgressAction.h"
#include "actions/TriggerAction.h"

#include "models/TasksTreeModel.h"

#include <QStyledItemDelegate>
#include <QStyleOptionButton>
#include <QHeaderView>
#include <QSortFilterProxyModel>

using namespace mv::util;

namespace mv::gui {

/** Tree view item delegate class for showing custom task progress user interface */
class ProgressItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with pointer to \p tasksTreeAction
     * @param tasksTreeAction Pointer to tasks tree action
     */
    explicit ProgressItemDelegate(TasksTreeAction* tasksTreeAction) :
        QStyledItemDelegate(tasksTreeAction),
        _tasksTreeAction(tasksTreeAction)
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
        auto item = _tasksTreeAction->getModel()->itemFromIndex(_tasksTreeAction->getFilterModel()->mapToSource(index).siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<AbstractTasksModel::ProgressItem*>(item)->getTaskAction().getProgressAction());
    }

private:
    TasksTreeAction* _tasksTreeAction;
};

/** Tree view item delegate class for showing custom task kill user interface */
class KillTaskItemDelegate : public QStyledItemDelegate {
public:

    /**
     * Construct with pointer to \p tasksTreeAction
     * @param tasksTreeAction Pointer to tasks tree action
     */
    explicit KillTaskItemDelegate(TasksTreeAction* tasksTreeAction) :
        QStyledItemDelegate(tasksTreeAction),
        _tasksTreeAction(tasksTreeAction)
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
        auto item = _tasksTreeAction->getModel()->itemFromIndex(_tasksTreeAction->getFilterModel()->mapToSource(index).siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (item == nullptr)
            return nullptr;

        return &(dynamic_cast<AbstractTasksModel::ProgressItem*>(item)->getTaskAction().getKillTaskAction());
    }

private:
    TasksTreeAction* _tasksTreeAction;
};

TasksTreeAction::TasksTreeAction(QObject* parent, const QString& title) :
    GroupAction(parent, title),
    _model(nullptr),
    _filterModel(nullptr),
    _treeAction(this, "Tasks"),
    _mayLoadTasksPlugin(true),
    _loadTasksPluginAction(this, "Plugin")
{
    setShowLabels(false);
    setIconByName("list-check");
    setDefaultWidgetFlag(NoMargins);

    addAction(&_treeAction, -1, [this](WidgetAction* action, QWidget* widget) -> void {
        Q_ASSERT(widget);

        if (!widget)
            return;

        widget->layout()->setContentsMargins(0, 0, 0, 0);

        _loadTasksPluginAction.setEnabled(mv::plugins().getPluginFactory("Tasks")->getNumberOfInstances() == 0);

        auto hierarchyWidget = widget->findChild<HierarchyWidget*>("HierarchyWidget");

        Q_ASSERT(hierarchyWidget);

        if (hierarchyWidget == nullptr)
            return;

        hierarchyWidget->setWindowIcon(StyledIcon("list-check"));

        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getTaskTypeFilterAction());
        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getTaskScopeFilterAction());
        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getTaskStatusFilterAction());
        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getTopLevelTasksOnlyAction());
        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getHideDisabledTasksFilterAction());
        hierarchyWidget->getFilterGroupAction().addAction(&_filterModel->getHideHiddenTasksFilterAction());

        hierarchyWidget->setHeaderHidden(false);

        if (_mayLoadTasksPlugin)
            hierarchyWidget->getToolbarAction().addAction(&_loadTasksPluginAction);

        auto& treeView = hierarchyWidget->getTreeView();

        treeView.setRootIsDecorated(true);

        treeView.setItemDelegateForColumn(static_cast<int>(AbstractTasksModel::Column::Progress), new ProgressItemDelegate(this));
        treeView.setItemDelegateForColumn(static_cast<int>(AbstractTasksModel::Column::Kill), new KillTaskItemDelegate(this));

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

        connect(_filterModel, &QSortFilterProxyModel::rowsInserted, widget, [this, &treeView]() -> void {
            openPersistentProgressEditorsRecursively(treeView);

            QCoreApplication::processEvents();
        });

        connect(_filterModel, &QSortFilterProxyModel::rowsAboutToBeRemoved, widget, [this, &treeView](const QModelIndex& parent, int first, int last) -> void {
            for (int rowIndex = first; rowIndex <= last; rowIndex++) {
                const auto index = _filterModel->index(rowIndex, 0, parent);

                const auto persistentEditorsIndexes = QModelIndexList({
                    index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
                    index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
                    });

                for (const auto& persistentEditorIndex : persistentEditorsIndexes)
                    if (treeView.isPersistentEditorOpen(persistentEditorIndex))
                        treeView.closePersistentEditor(persistentEditorIndex);
            }
        });

        const auto updateTreeView = [this, &treeView]() -> void {
            treeView.setRootIsDecorated(hasAgregateTasks());
        };

        updateTreeView();

        connect(_filterModel, &QAbstractItemModel::rowsInserted, widget, updateTreeView);
        connect(_filterModel, &QAbstractItemModel::rowsRemoved, widget, updateTreeView);
        connect(_filterModel, &QAbstractItemModel::layoutChanged, widget, updateTreeView);

        openPersistentProgressEditorsRecursively(treeView);

        if (_widgetConfigurationFunction)
            _widgetConfigurationFunction(action, widget);
    });

    _loadTasksPluginAction.setIconByName("window-maximize");
    _loadTasksPluginAction.setDefaultWidgetFlags(TriggerAction::WidgetFlag::Icon);
    _loadTasksPluginAction.setToolTip("Load tasks plugin");

    connect(&_loadTasksPluginAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::plugins().requestViewPlugin("list-check", nullptr, DockAreaFlag::Bottom);

        _loadTasksPluginAction.setEnabled(false);
    });
}

AbstractTasksModel* TasksTreeAction::getModel()
{
    return _model;
}

TasksFilterModel* TasksTreeAction::getFilterModel()
{
    return _filterModel;
}

void TasksTreeAction::initialize(AbstractTasksModel* model, TasksFilterModel* filterModel, const QString& itemTypeName, WidgetConfigurationFunction widgetConfigurationFunction /*= WidgetConfigurationFunction()*/, bool mayLoadTasksPlugin /*= true*/)
{
    Q_ASSERT(model);
    Q_ASSERT(filterModel);

    if (!model || !filterModel)
        return;

    _model = model;
    _filterModel = filterModel;
    _widgetConfigurationFunction = widgetConfigurationFunction;
    _mayLoadTasksPlugin = mayLoadTasksPlugin;

    _treeAction.initialize(_model, _filterModel, itemTypeName);
}


void TasksTreeAction::openPersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent /*= QModelIndex()*/)
{
    Q_ASSERT(_filterModel);

    for (int rowIndex = 0; rowIndex < _filterModel->rowCount(parent); ++rowIndex) {
        const auto index = _filterModel->index(rowIndex, 0, parent);

        const auto persistentEditorsIndexes = QModelIndexList({
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
            });

        for (const auto& persistentEditorIndex : persistentEditorsIndexes)
            if (!itemView.isPersistentEditorOpen(persistentEditorIndex))
                itemView.openPersistentEditor(persistentEditorIndex);

        if (_filterModel->hasChildren(index))
            openPersistentProgressEditorsRecursively(itemView, index);
    }
}

void TasksTreeAction::closePersistentProgressEditorsRecursively(QAbstractItemView& itemView, const QModelIndex& parent /*= QModelIndex()*/)
{
    Q_ASSERT(_filterModel);

    for (int rowIndex = 0; rowIndex < _filterModel->rowCount(parent); ++rowIndex) {
        const auto index = _filterModel->index(rowIndex, 0, parent);

        const auto persistentEditorsIndexes = QModelIndexList({
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Progress)),
            index.siblingAtColumn(static_cast<int>(AbstractTasksModel::Column::Kill))
            });

        for (const auto& persistentEditorIndex : persistentEditorsIndexes)
            if (itemView.isPersistentEditorOpen(persistentEditorIndex))
                itemView.closePersistentEditor(persistentEditorIndex);

        if (_filterModel->hasChildren(index))
            closePersistentProgressEditorsRecursively(itemView, index);
    }
}

bool TasksTreeAction::hasAgregateTasks() const
{
    Q_ASSERT(_filterModel);

    const auto matches = _filterModel->match(_filterModel->index(0, static_cast<int>(AbstractTasksModel::Column::ProgressMode)), Qt::EditRole, static_cast<int>(Task::ProgressMode::Aggregate), -1, Qt::MatchExactly | Qt::MatchRecursive);

    return !matches.isEmpty();
}

}