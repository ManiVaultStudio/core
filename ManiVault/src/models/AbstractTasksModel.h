// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "StandardItemModel.h"
#include "Task.h"

#include "actions/StringAction.h"
#include "actions/TaskAction.h"

#include <QList>
#include <QStandardItem>

namespace mv
{

/**
 * Tasks model class
 *
 * Standard item model class for tasks
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractTasksModel : public StandardItemModel
{
    Q_OBJECT

public:

    /** Task columns */
    enum class Column {
        Name,                   /** Name of the task */
        Enabled,                /** Whether the task is enabled, disabled tasks are not included in task aggregation */
        Visible,                /** Whether the task is visible in the user interface */
        Progress,               /** Task progress */
        ProgressDescription,    /** Progress description */
        ProgressText,           /** Progress text */
        Status,                 /** Status of the task */
        ProgressMode,           /** Progress mode (manual, subtasks or aggregate) */
        ID,                     /** Globally unique identifier of the task */
        ParentID,               /** Globally unique identifier of the parent task (empty string if not a child task) */
        Type,                   /** Task type */
        GuiScopes,              /** Task GUI scopes */
        MayKill,                /** Task may kill boolean */
        Kill,                   /** Column for killing a task */

        Count
    };

    /** Header strings for several data roles */
    struct ColumHeaderInfo {
        QString     _display;   /** Header string for display role */
        QString     _edit;      /** Header string for edit role */
        QString     _tooltip;   /** Header string for tooltip role */
    };

    /** Column name and tooltip */
    static QMap<Column, ColumHeaderInfo> columnInfo;

protected:

    /** Header standard model item class */
    class HeaderItem : public QStandardItem {
    public:

        /**
         * Construct with \p columHeaderInfo
         * @param columHeaderInfo Column header info
         */
        HeaderItem(const ColumHeaderInfo& columHeaderInfo);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

    private:
        ColumHeaderInfo     _columHeaderInfo;   /** Column header info */
    };

public:

    /** Base standard model item class for task */
    class CORE_EXPORT Item : public QStandardItem, public QObject {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         * @param editable Whether the model item is editable or not
         */
        Item(Task* task, bool editable = false);

        /**
         * Get task
         * return Pointer to task to display item for
         */
        Task* getTask() const;

    private:
        Task*   _task;      /** Pointer to task to display item for */
    };

    /** Standard model item class for displaying the task name */
    class CORE_EXPORT NameItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        NameItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /** Set model data to \p value for \p role */
        void setData(const QVariant& value, int role /* = Qt::UserRole + 1 */) override;

        /**
         * Get string action
         * @return String action for use in item delegates
         */
        gui::StringAction& getStringAction() {
            return _stringAction;
        }

    private:
        gui::StringAction   _stringAction;    /** String action for use in item delegates */
    };

protected:

    /** Standard model item class for displaying whether the task is enabled or not */
    class EnabledItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        EnabledItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the task is visible or not */
    class VisibleItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        VisibleItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

public:

    /** Standard model item class for displaying the task progress */
    class CORE_EXPORT ProgressItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        ProgressItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;

        /**
         * Get task action
         * @return Task action for use in item delegate (its built-in progress action)
         */
        gui::TaskAction& getTaskAction() {
            return _taskAction;
        }

    private:
        gui::TaskAction     _taskAction;    /** Task action for use in item delegate (uses its built-in progress action) */
    };

protected:

    /** Standard model item class for displaying the progress description */
    class CORE_EXPORT ProgressDescriptionItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        ProgressDescriptionItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the progress text */
    class CORE_EXPORT ProgressTextItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        ProgressTextItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task status */
    class CORE_EXPORT StatusItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        StatusItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the progress mode */
    class CORE_EXPORT ProgressModeItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        ProgressModeItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task identifier */
    class CORE_EXPORT IdItem final : public Item {
    public:

        /** Use base task item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the parent task identifier */
    class CORE_EXPORT ParentIdItem final : public Item {
    public:

        /** Use base task item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task type */
    class CORE_EXPORT TypeItem final : public Item {
    public:

        /** Use base task item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task GUI scopes */
    class CORE_EXPORT GuiScopesItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        GuiScopesItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying whether the task may be killed */
    class CORE_EXPORT MayKillItem final : public Item {
    public:

        /** Use base task item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for killing a task */
    class CORE_EXPORT KillItem final : public Item {
    public:

        /**
         * Construct with \p task
         * @param task Pointer to task to display item for
         */
        KillItem(Task* task);

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Convenience class for combining task items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct row with \p task
         * @param task Pointer to row task
         */
        Row(Task* task) : QList<QStandardItem*>()
        {
            append(new NameItem(task));
            append(new EnabledItem(task));
            append(new VisibleItem(task));
            append(new ProgressItem(task));
            append(new ProgressDescriptionItem(task));
            append(new ProgressTextItem(task));
            append(new StatusItem(task));
            append(new ProgressModeItem(task));
            append(new IdItem(task));
            append(new ParentIdItem(task));
            append(new TypeItem(task));
            append(new GuiScopesItem(task));
            append(new MayKillItem(task));
            append(new KillItem(task));
        }

    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractTasksModel(QObject* parent = nullptr);

    /**
     * Get item from \p task
     * @return Pointer to found item, nullptr otherwise
     */
    QStandardItem* itemFromTask(Task* task) const;

private:

    /**
     * Add \p task to the model (this method is called when a task is added to the manager)
     * @param task Pointer to task to add
     */
    virtual void addTask(Task* task) = 0;

    /**
     * Remove \p task from the model (this method is called when a task is about to be removed from the manager)
     * @param task Pointer to task to remove
     */
    void removeTask(Task* task);

    friend class Item;
};

}
