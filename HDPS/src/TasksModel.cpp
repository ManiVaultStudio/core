// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksModel.h"

#include <util/Exception.h>

using namespace hdps::gui;

#ifdef _DEBUG
    #define TASKS_MODEL_VERBOSE
#endif

namespace hdps
{

using namespace util;

TasksModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant TasksModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return _columHeaderInfo._display;

        case Qt::EditRole:
            return _columHeaderInfo._edit;

        case Qt::ToolTipRole:
            return _columHeaderInfo._tooltip;

        default:
            break;
    }

    return QVariant();
}

TasksModel::Item::Item(Task* task, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _task(task)
{
    Q_ASSERT(_task != nullptr);

    setEditable(editable);
    setDropEnabled(true);
}

Task* TasksModel::Item::getTask() const
{
    return _task;
}

TasksModel::NameItem::NameItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::nameChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::descriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant TasksModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getName();

        case Qt::ToolTipRole:
            return getTask()->getDescription();

        default:
            break;
    }

    return Item::data(role);
}

void TasksModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getTask()->setName(value.toString());
            break;

        default:
            Item::setData(value, role);
    }
}

TasksModel::ProgressItem::ProgressItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::progressChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant TasksModel::ProgressItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getTask()->getProgress();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toFloat(), 'f', 2);

        case Qt::ToolTipRole:
            return "Task progress: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

TasksModel::ProgressDescriptionItem::ProgressDescriptionItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::currentSubtaskDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant TasksModel::ProgressDescriptionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getProgressDescription();

        case Qt::ToolTipRole:
            return "Current sub-task description: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant TasksModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getId();

        case Qt::ToolTipRole:
            return "Task globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant TasksModel::ParentIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getId();

        case Qt::ToolTipRole:
            return "Parent task globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant TasksModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    const auto taskTypeString = QString(getTask()->metaObject()->className());

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return taskTypeString;

        case Qt::ToolTipRole:
            return "Task type: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

TasksModel::StatusItem::StatusItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });

    const auto progressIndex = index().siblingAtColumn(static_cast<int>(Column::Progress));

    emit model()->dataChanged(progressIndex, progressIndex);
}

QVariant TasksModel::StatusItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getTask()->getStatus());

        case Qt::DisplayRole:
            return Task::statusNames[static_cast<Task::Status>(data(Qt::EditRole).toInt())];

        case Qt::ToolTipRole:
            return "Task is: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

TasksModel::Row::Row(Task* task) :
    QList<QStandardItem*>()
{
    append(new NameItem(task));
    append(new ProgressItem(task));
    append(new ProgressDescriptionItem(task));
    append(new IdItem(task));
    append(new ParentIdItem(task));
    append(new TypeItem(task));
    append(new StatusItem(task));
}

QMap<TasksModel::Column, TasksModel::ColumHeaderInfo> TasksModel::columnInfo = QMap<TasksModel::Column, TasksModel::ColumHeaderInfo>({
    { TasksModel::Column::Name, { "Name" , "Name", "Name of the task" } },
    { TasksModel::Column::Progress, { "Progress" , "Progress", "Task progress" } },
    { TasksModel::Column::ProgressDescription, { "Progress description" , "Progress description", "Progress description" } },
    { TasksModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the task" } },
    { TasksModel::Column::ParentID, { "Parent ID",  "Parent ID", "Globally unique identifier of the parent task" } },
    { TasksModel::Column::Type, { "Type",  "Type", "Type of task" } },
    { TasksModel::Column::Status, { "Status",  "Status", "Status of the task" } }
});

TasksModel::TasksModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&tasks(), &AbstractTaskManager::taskAdded, this, &TasksModel::taskAddedToTaskManager);
    connect(&tasks(), &AbstractTaskManager::taskAboutToBeRemoved, this, &TasksModel::taskAboutToBeRemovedFromTaskManager);
}

void TasksModel::taskAddedToTaskManager(Task* task)
{
    try {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a nullptr");

        appendRow(Row(task));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add task to tasks model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add task to tasks model");
    }
}

void TasksModel::taskAboutToBeRemovedFromTaskManager(Task* task)
{
    try {
        Q_ASSERT(task != nullptr);

        if (task == nullptr)
            throw std::runtime_error("Task may not be a nullptr");

        const auto matches = match(QModelIndex(), Qt::EditRole, task->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

        if (matches.empty())
            throw std::runtime_error(QString("%1 not found").arg(task->getName()).toStdString());

        removeRow(matches.first().row());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove task from tasks model", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove task from tasks model");
    }
}

}
