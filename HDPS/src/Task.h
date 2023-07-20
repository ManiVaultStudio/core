// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include <QObject>
#include <QBitArray>

namespace hdps {

class TaskHandler;

/**
 * Task class
 *
 * Convenience class for managing a task.
 *
 * Task progress can be modified in two ways:
 *  - Setting progress directly setProgress(...)
 *  - Setting a number of task items via setNumberOfItems(...) and flagging items as finished 
 *    with setItemFinished(...), the percentage is then updated automatically
 *
 * @author Thomas Kroes
 */
class Task final : public QObject, public util::Serializable
{
    Q_OBJECT

public:

    /** Task: */
    enum class Status {
        Undefined = -1, /** status is undefined */
        Idle,           /** is idle */
        Running,        /** is currently running */
        Finished,       /** has finished successfully */
        Aborted         /** has been aborted */
    };

public:

    /**
    * Construct progress with \p parent object, \p name, initial \p status and possibly a \p taskHandler
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param taskHandler Pointer to task handler (tasks will be displayed in the global task window when nullptr)
    */
    Task(QObject* parent, const QString& name, const Status& status = Status::Idle, TaskHandler* taskHandler = nullptr);

    /** Remove from task manager when destructed */
    ~Task();

public: // Name and description

    /** Get task name */
    QString getName() const;

    /**
     * Set task name to \p name
     * @param name Name of the task
     */
    void setName(const QString& name);

    /** Gets the task description */
    QString getDescription() const;

    /**
     * Sets the task description to \p description
     * @param description Task description
     */
    void setDescription(const QString& description);

public: // Status

    /** Get task status */
    Status getStatus() const;

    /** Check if task status is idle */
    bool isIdle() const;

    /** Check if task status is idle */
    bool isRunning() const;

    /** Check if task status is idle */
    bool isFinished() const;

    /** Check if task status is idle */
    bool isAborted() const;

    /**
     * Set task status to \p status
     * @param status Task status
     */
    void setStatus(const Status& status);

    /** Set task status to idle */
    void setIdle();

    /** Set task status to running */
    void setRunning();

    /** Set task status to finished */
    void setFinished();

    /** Set task status to aborted */
    void setAborted();

public: // Progress

    /** Gets the task progress [0, 1] */
    float getProgress() const;

    /**
     * Sets the task progress to \p progress
     * @param progress Task progress
     */
    void setProgress(float progress);

    /**
     * Set the number of items to \p numberOfItems
     * @param numberOfItems Number of task items
     */
    void setNumberOfItems(std::uint32_t numberOfItems);

    /**
     * Flag item with \p itemIndex as finished
     * @param itemIndex Index of the item
     */
    void setItemFinished(std::uint32_t itemIndex);

public: // Start

    /**
     * Starts the task and initializes with the \p numberOfItems
     * @param numberOfItems Number of task items
     */
    void start(std::uint32_t numberOfItems);

signals:

    /**
     * Signals that the task name changed to \p name
     * @param name Modified name
     */
    void nameChanged(const QString& name);

    /**
     * Signals that the task description changed to \p description
     * @param description Modified description
     */
    void descriptionChanged(const QString& description);

    /**
     * Signals that the task status changed to \p status
     * @param status Modified name
     */
    void statusChanged(const Status& status);

    /**
     * Signals that the task progress changed to \p progress
     * @param progress Modified progress
     */
    void progressChanged(float progress);

    /**
     * Signals that items changed to \p items
     * @param items Modified items
     */
    void itemsChanged(const QBitArray& items);

private:
    QString     _name;          /** Task name */
    QString     _description;   /** Task description */
    Status      _status;        /** Task status */
    float       _progress;      /** Task progress */
    QBitArray   _items;         /** Task items status bit array */
};

}
