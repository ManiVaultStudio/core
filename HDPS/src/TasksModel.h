// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "CoreInterface.h"
#include "Task.h"

#include <QStandardItemModel>

namespace hdps
{

/**
 * Tasks model class
 *
 * Standard item model class for tasks
 *
 * @author Thomas Kroes
 */
class TasksModel : public QStandardItemModel
{
    Q_OBJECT

public:

    /** Task columns */
    enum class Column {
        Name,                   /** Name of the task */
        Progress,               /** Task progress */
        ProgressDescription,    /** Progress description */
        ID,                     /** Globally unique identifier of the task */
        ParentID,               /** Globally unique identifier of the parent task (empty string if not a child task) */
        Type,                   /** Task type string */
        Status,                 /** Status of the task */

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
    class Item : public QStandardItem, public QObject {
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
        Task*   _task;  /** Pointer to task to display item for */
    };

protected:

    /** Standard model item class for displaying the task name */
    class NameItem final : public Item {
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
    };

    /** Standard model item class for displaying the task progress */
    class ProgressItem final : public Item {
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
    };

    /** Standard model item class for displaying the progress description */
    class ProgressDescriptionItem final : public Item {
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

    /** Standard model item class for displaying the task identifier */
    class IdItem final : public Item {
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
    class ParentIdItem final : public Item {
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
    class TypeItem final : public Item {
    public:

        /** Use base task item constructor */
        using Item::Item;

        /**
         * Get model data for \p role
         * @return Data for \p role in variant form
         */
        QVariant data(int role = Qt::UserRole + 1) const override;
    };

    /** Standard model item class for displaying the task status */
    class StatusItem final : public Item {
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

    /** Convenience class for combining task items in a row */
    class Row final : public QList<QStandardItem*>
    {
    public:

        /**
         * Construct row with \p task
         * @param task Pointer to row task
         */
        Row(Task* task);
    };

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    TasksModel(QObject* parent = nullptr);

private:

    /**
     * Invoked when \p task is added to the task manager
     * @param task Pointer to task that was added
     */
    void taskAddedToTaskManager(Task* task);

    /**
     * Invoked when \p task is about to be removed from the task manager
     * @param task Pointer to task that is about to be removed
     */
    void taskAboutToBeRemovedFromTaskManager(Task* task);

    friend class Item;
};

}