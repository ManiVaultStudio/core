// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include <QObject>
#include <QBitArray>

namespace hdps {

class AbstractTaskHandler;

/**
 * Task class
 *
 * Convenience class for managing a task.
 *
 * Task progress can be modified in two ways:
 * 
 * 1. By setting task progress directly setProgress(...)
 *    
 *  - Setting progress directly setProgress(...)
 *  - Setting a number of task items via setNumberOfItems(...) and flagging items as finished 
 *    with setItemFinished(...), the percentage is then updated automatically
 *
 * @author Thomas Kroes
 */
class Task : public QObject, public util::Serializable
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

    static QMap<Status, QString> statusNames;

public:

    /**
    * Construct task with \p parent object, \p name, initial \p status and possibly a \p taskHandler
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param handler Pointer to task handler
    */
    Task(QObject* parent, const QString& name, const Status& status = Status::Idle, AbstractTaskHandler* handler = nullptr);

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

public:

    /**
     * Get task handler
     * @return Pointer to task handler
     */
    AbstractTaskHandler* getHandler();

    /**
     * Set task handler to \p handler
     * @param handler Pointer to task handler
     */
    void setHandler(AbstractTaskHandler* handler);

public: // Progress

    /** Gets the task progress [0, 1] */
    float getProgress() const;

    /**
     * Sets the task progress to \p progress
     * @param progress Task progress, clamped to [0, 1]
     * @param subtaskDescription Subtask description associated with the progress update
     */
    void setProgress(float progress, const QString& subtaskDescription = "");

public: // Subtasks

    /**
     * Set the number of subtasks to \p numberOfSubtasks
     * @param numberOfSubtasks Number of subtasks
     */
    void setNumberOfSubtasks(std::uint32_t numberOfSubtasks);

    /**
     * Flag item with \p subtaskIndex as finished
     * @param subtaskIndex Index of the subtask
     */
    void setSubtaskFinished(std::uint32_t subtaskIndex);

    /**
     * Set subtasks descriptions to \p subtasksDescriptions
     * @param subtasksDescriptions Subtasks descriptions
     */
    void setSubtasksDescriptions(const QStringList& subtasksDescriptions);

    /**
     * Set subtask description to \p subtaskDescription for \p subtaskIndex
     * @param subtaskIndex Subtask index to set the description for
     * @param subtaskDescription Description of the subtask
     */
    void setSubtaskDescription(std::uint32_t subtaskIndex, const QString& subtaskDescription);

    /**
     * Get current subtask description
     * @return Description of the current subtask
     */
    QString getCurrentSubtaskDescription() const;

    /**
     * Set current subtask description to \p currentSubtaskDescription
     * @param currentSubtaskDescription Description of the current subtask
     */
    void setCurrentSubtaskDescription(const QString& currentSubtaskDescription);

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
     * Signals that the task handler changed to \p handler
     * @param handler Task handler
     */
    void handlerChanged(AbstractTaskHandler* handler);

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
     * Signals that subtasks changed to \p subtasks
     * @param subtasks Modified subtasks
     */
    void subtasksChanged(const QBitArray& subtasks);

    /**
     * Signals that subtasks descriptions changed to \p subtasksDescriptions
     * @param subtasksDescriptions Modified subtasks descriptions
     */
    void subtasksDescriptionsChanged(const QStringList& subtasksDescriptions);

    /**
     * Signals that current subtask description changed to \p currentSubtaskDescription
     * @param currentSubtaskDescription Modified description of the current subtask
     */
    void currentSubtaskDescriptionChanged(const QString& currentSubtaskDescription);

private:
    QString                 _name;                          /** Task name */
    QString                 _description;                   /** Task description */
    Status                  _status;                        /** Task status */
    AbstractTaskHandler*    _handler;                       /** Task handler */
    float                   _progress;                      /** Task progress */
    QBitArray               _subtasks;                      /** Subtasks status */
    QStringList             _subtasksDescriptions;          /** Subtasks descriptions */
    QString                 _currentSubtaskDescription;     /** Current item description */
};

}
