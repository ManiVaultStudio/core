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
        Undefined = -1,         /** status is undefined */
        Idle,                   /** is idle */
        Running,                /** is currently running */
        RunningIndeterminate,   /** is currently running, but it's operating time is not known */
        Finished,               /** has finished successfully */
        Aborted                 /** has been aborted */
    };

    static QMap<Status, QString> statusNames;

    /** Progress is tracked by: */
    enum class ProgressMode {
        Manual,     /** setting progress percentage manually */
        Subtasks    /** setting a number of subtasks and flagging subtasks as finished (progress percentage is computed automatically) */
    };

public:

    /**
    * Construct task with \p parent object, \p name, initial \p status and possibly a \p taskHandler
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param handler Pointer to task handler
    */
    Task(QObject* parent, const QString& name, const Status& status = Status::RunningIndeterminate, AbstractTaskHandler* handler = nullptr);

    /** Remove from task manager when destructed */
    ~Task();

public: // Name and description

    /** Get task name */
    virtual QString getName() const final;

    /**
     * Set task name to \p name
     * @param name Name of the task
     */
    virtual void setName(const QString& name) final;

    /** Gets the task description */
    virtual QString getDescription() const final;

    /**
     * Sets the task description to \p description
     * @param description Task description
     */
    virtual void setDescription(const QString& description) final;

public: // Status

    /** Get task status */
    virtual Status getStatus() const final;

    /** Check if task is idle */
    virtual bool isIdle() const final;

    /** Check if task is running */
    virtual bool isRunning() const final;

    /** Check if task is running indeterminate */
    virtual bool isRunningIndeterminate() const final;

    /** Check if task is idle */
    virtual bool isFinished() const final;

    /** Check if task is idle */
    virtual bool isAborted() const final;

    /**
     * Set task status to \p status
     * @param status Task status
     */
    virtual void setStatus(const Status& status) final;

    /** Set task status to idle */
    virtual void setIdle() final;

    /** Set task status to running */
    virtual void setRunning() final;

    /** Set task status to running indeterminate */
    virtual void setRunningIndeterminate() final;

    /** Set task status to finished */
    virtual void setFinished() final;

    /** Set task status to aborted */
    virtual void setAborted() final;

public:

    /**
     * Get task handler
     * @return Pointer to task handler
     */
    virtual AbstractTaskHandler* getHandler() final;

    /**
     * Set task handler to \p handler
     * @param handler Pointer to task handler
     */
    virtual void setHandler(AbstractTaskHandler* handler) final;

public: // Progress mode

    /**
     * Get progress mode
     * @return Progress mode enum
     */
    virtual ProgressMode getProgressMode() const final;

    /**
     * Sets progress mode to to \p progressMode
     * @param progressMode Progress mode
     */
    virtual void setProgressMode(const ProgressMode& progressMode) final;

public: // Manual

    /** Gets the task progress [0, 1] */
    virtual float getProgress() const final;

    /**
     * Sets the task progress percentage to \p progress
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Manual
     * @param progress Task progress, clamped to [0, 1]
     * @param subtaskDescription Subtask description associated with the progress update
     */
    virtual void setProgress(float progress, const QString& subtaskDescription = "");

public: // Subtasks

    /**
     * Set the number of subtasks to \p numberOfSubtasks
     * This method sets the Task#_progressMode to ProgressMode::Subtasks
     * @param numberOfSubtasks Number of subtasks
     */
    virtual void setNumberOfSubtasks(std::uint32_t numberOfSubtasks) final;

    /**
     * Flag item with \p subtaskIndex as finished, the progress percentage will be computed automatically
     * Assumes the number of tasks has been set prior with Task::setNumberOfSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Index of the subtask
     */
    virtual void setSubtaskFinished(std::uint32_t subtaskIndex) final;

    /**
     * Set subtasks descriptions to \p subtasksDescriptions
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtasksDescriptions Subtasks descriptions
     */
    virtual void setSubtasksDescriptions(const QStringList& subtasksDescriptions) final;

    /**
     * Set subtask description to \p subtaskDescription for \p subtaskIndex
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Subtask index to set the description for
     * @param subtaskDescription Description of the subtask
     */
    virtual void setSubtaskDescription(std::uint32_t subtaskIndex, const QString& subtaskDescription) final;

    /**
     * Starts the task and initializes with the \p numberOfItems
     * @param numberOfItems Number of task items
     */
    virtual void start(std::uint32_t numberOfItems) final;

public: // Progress description

    /**
     * Get progress description
     * @return Progress description
     */
    virtual QString getProgressDescription() const final;

    /**
     * Set progress description to \p progressDescription
     * @param progressDescription Progress description
     */
    virtual void setProgressDescription(const QString& progressDescription) final;

private:

    /** Updates the progress percentage */
    void updateProgress();

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
     * Signals that the task progress mode to \p progressMode
     * @param progressMode Modified progress mode
     */
    void progressModeChanged(const ProgressMode& progressMode);

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
    QString                 _name;                      /** Task name */
    QString                 _description;               /** Task description */
    Status                  _status;                    /** Task status */
    AbstractTaskHandler*    _handler;                   /** Task handler */
    ProgressMode            _progressMode;              /** The way progress is recorded */
    float                   _progress;                  /** Task progress */
    QBitArray               _subtasks;                  /** Subtasks status */
    QStringList             _subtasksDescriptions;      /** Subtasks descriptions */
    QString                 _progressDescription;       /** Current item description */
};

}
