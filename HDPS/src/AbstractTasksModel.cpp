// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AbstractTasksModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <util/Exception.h>

#ifdef _DEBUG
    #define ABSTRACT_TASKS_MODEL_VERBOSE
#endif

namespace hdps
{

using namespace util;

AbstractTasksModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant AbstractTasksModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
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

AbstractTasksModel::Item::Item(Task* task, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _task(task)
{
    Q_ASSERT(_task != nullptr);

    setEditable(editable);
    setDropEnabled(true);
}

Task* AbstractTasksModel::Item::getTask() const
{
    return _task;
}

AbstractTasksModel::ExpandCollapseItem::ExpandCollapseItem(Task* task) :
    Item(task)
{
    setSizeHint(QSize(0, 0));
}

AbstractTasksModel::StatusItem::StatusItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();

        const auto progressIndex = index().siblingAtColumn(static_cast<int>(Column::Progress));

        emit model()->dataChanged(progressIndex, progressIndex);
    });
}

QVariant AbstractTasksModel::StatusItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getTask()->getStatus());

        case Qt::DisplayRole:
            return "";

        case Qt::ToolTipRole:
            return "Task is: " + Task::statusNames[getTask()->getStatus()];

        case Qt::DecorationRole:
        {
            auto& fontAwesome = Application::getIconFont("FontAwesome");

            switch (getTask()->getStatus())
            {
                case Task::Status::Idle:
                    return fontAwesome.getIcon("clock");

                case Task::Status::Running:
                    return fontAwesome.getIcon("play");

                case Task::Status::RunningIndeterminate:
                    return fontAwesome.getIcon("play-circle");

                case Task::Status::Finished:
                    return fontAwesome.getIcon("check-circle");

                case Task::Status::Aborted:
                    return fontAwesome.getIcon("bomb");

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::NameItem::NameItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::nameChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::descriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getName();

        case Qt::ToolTipRole:
            return "Task name: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

void AbstractTasksModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getTask()->setName(value.toString());
            break;

        default:
            Item::setData(value, role);
    }
}

AbstractTasksModel::EnabledItem::EnabledItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::enabledChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::EnabledItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getTask()->getEnabled();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return QString("Task is %1enabled").arg(data(Qt::EditRole).toBool() ? "" : "not ");

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::VisibleItem::VisibleItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::visibileChanged, this, [this]() -> void {
        emitDataChanged();
        });
}

QVariant AbstractTasksModel::VisibleItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getTask()->getVisible();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return QString("Task is %1visible").arg(data(Qt::EditRole).toBool() ? "" : "not ");

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::ProgressItem::ProgressItem(Task* task) :
    Item(task, true),
    _taskAction(this, "Task")
{
    _taskAction.setTask(getTask());

    connect(getTask(), &Task::progressChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::progressDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::ProgressItem::data(int role /*= Qt::UserRole + 1*/) const
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

AbstractTasksModel::ProgressDescriptionItem::ProgressDescriptionItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::progressDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::ProgressDescriptionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getProgressDescription();

        case Qt::ToolTipRole:
            return "Progress description: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::ProgressTextItem::ProgressTextItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::progressChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::progressDescriptionChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

AbstractTasksModel::ProgressModeItem::ProgressModeItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::progressModeChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::ProgressModeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getTask()->getProgressMode());

        case Qt::DisplayRole:
            return Task::progressModeNames[static_cast<Task::ProgressMode>(data(Qt::EditRole).toInt())];

        case Qt::ToolTipRole:
            return "Progress mode: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractTasksModel::ProgressTextItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->getProgressText();

        case Qt::ToolTipRole:
            return "Progress text: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractTasksModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
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

QVariant AbstractTasksModel::ParentIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getTask()->hasParentTask() ? getTask()->getParentTask()->getId() : "";

        case Qt::ToolTipRole:
            return "Parent task globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractTasksModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    const auto taskTypeString = getTask()->getTypeName(true);

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

AbstractTasksModel::ScopeItem::ScopeItem(Task* task) :
    Item(task, false)
{
    connect(getTask(), &Task::scopeChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::ScopeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    auto taskScopeString = Task::scopeNames[getTask()->getScope()];

    switch (role) {
        case Qt::EditRole:
            return static_cast<std::int32_t>(getTask()->getScope());

        case Qt::DisplayRole:
            return taskScopeString;

        case Qt::ToolTipRole:
            return "Task scope: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractTasksModel::MayKillItem::data(int role /*= Qt::UserRole + 1*/) const
{
    auto taskTypeString = QString(getTask()->metaObject()->className());

    taskTypeString.replace("hdps::", "");

    switch (role) {
        case Qt::EditRole:
            return getTask()->getMayKill();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return QString("Task may %1be killed").arg(data(Qt::EditRole).toBool() ? "" : "not ");

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::KillItem::KillItem(Task* task) :
    Item(task)
{
    connect(getTask(), &Task::statusChanged, this, [this]() -> void {
        emitDataChanged();
    });

    connect(getTask(), &Task::mayKillChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractTasksModel::KillItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            break;

        case Qt::ToolTipRole:
            return QString("Kill %1").arg(getTask()->getName());

        default:
            break;
    }

    return Item::data(role);
}

AbstractTasksModel::Row::Row(Task* task) :
    QList<QStandardItem*>()
{
    append(new ExpandCollapseItem(task));
    append(new StatusItem(task));
    append(new NameItem(task));
    append(new EnabledItem(task));
    append(new VisibleItem(task));
    append(new ProgressItem(task));
    append(new ProgressDescriptionItem(task));
    append(new ProgressTextItem(task));
    append(new ProgressModeItem(task));
    append(new IdItem(task));
    append(new ParentIdItem(task));
    append(new TypeItem(task));
    append(new ScopeItem(task));
    append(new MayKillItem(task));
    append(new KillItem(task));
}

QMap<AbstractTasksModel::Column, AbstractTasksModel::ColumHeaderInfo> AbstractTasksModel::columnInfo = QMap<AbstractTasksModel::Column, AbstractTasksModel::ColumHeaderInfo>({
    { AbstractTasksModel::Column::ExpandCollapse, { "",  "Root", "Expand/collapse" } },
    { AbstractTasksModel::Column::Status, { "",  "Status", "Status of the task" } },
    { AbstractTasksModel::Column::Name, { "Name" , "Name", "Name of the task" } },
    { AbstractTasksModel::Column::Enabled, { "" , "Enabled", "Whether the task is enabled or not" } },
    { AbstractTasksModel::Column::Visible, { "" , "Visible", "Whether the task is visible or not" } },
    { AbstractTasksModel::Column::Progress, { "Progress" , "Progress", "Task progress" } },
    { AbstractTasksModel::Column::ProgressDescription, { "Progress description" , "Progress description", "Progress description" } },
    { AbstractTasksModel::Column::ProgressText, { "Progress text" , "Progress text", "Progress text" } },
    { AbstractTasksModel::Column::ProgressMode, { "Progress mode" , "Progress mode", "Progress mode" } },
    { AbstractTasksModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the task" } },
    { AbstractTasksModel::Column::ParentID, { "Parent ID",  "Parent ID", "Globally unique identifier of the parent task" } },
    { AbstractTasksModel::Column::Type, { "Type",  "Type", "Type of task" } },
    { AbstractTasksModel::Column::Scope, { "Scope",  "Scope", "Task scope" } },
    { AbstractTasksModel::Column::MayKill, { "May kill",  "May kill", "Whether the task may be killed or not" } },
    { AbstractTasksModel::Column::Kill, { "",  "Kill", "Kill the task" } }
});

AbstractTasksModel::AbstractTasksModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));
}

QStandardItem* AbstractTasksModel::itemFromTask(Task* task) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID)), Qt::EditRole, task->getId(), 1, Qt::MatchExactly | Qt::MatchRecursive);

    if (matches.isEmpty())
        throw std::runtime_error(QString("%1 not found").arg(task->getParentTask()->getName()).toStdString());

    auto item = itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::ExpandCollapse)));

    Q_ASSERT(item != nullptr);

    if (item == nullptr)
        throw std::runtime_error("Parent standard item may not be a nullptr");

    return item;
}

}
