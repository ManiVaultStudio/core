// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include <QObject>
#include <QBitArray>
#include <QTimer>
#include <QIcon>
#include <QSet>

namespace mv {

class AbstractTaskHandler;

/**
 * Task class
 *
 * Convenience class for managing a task.
 *
 * Task progress can be determined in three ways (see Task::setProgressMode() and Task::getProgressMode()):
 *  - Setting progress directly setProgress(...)
 *  - Setting sub tasks items via one of the overloads of Task::setSubTasks() and flagging items as finished 
 *    with Task::setSubtaskFinished(), the percentage is then updated automatically
 *  - Computing the combined progress of child tasks using aggregation (initialize with parent task or use Task::setParentTask())
 * 
 * Tasks have a scope which defines how the content is presented in the user interface (see Task::setScope() and Task::getScope())
 *  - All background tasks are aggregated into one overarching task and presented in the status bar
 *  - Foreground tasks automatically show up in a popup at the right of the status bar 
 *  - Modal tasks show up in a modal tasks dialog which blocks all other user interaction
 * 
 * There is no need to create a user interface to kill tasks, all scopes have controls to kill a task. Connect to the Task::requestAbort() signal
 * to be notified when a task was aborted so that further action can be taken.
 * 
 * Special notes:
 *  - Tasks should work cross threads (this has only been tested using QThread though)
 *  - For a more detail exploration of all tasks in the system, a tasks view system plugin is available
 *  - In case of a tasks hierarchy, all task objects should be in the same QThread context 
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT Task : public QObject, public util::Serializable
{
    Q_OBJECT

public:

    /** Describes the configuration options */
    enum class ConfigurationFlag {
        OverrideAggregateStatus = 0x00001,      /** Status is not automatically based on child tasks but will be set manually */

        Default = 0
    };

    /** Task: */
    enum class Status {
        Undefined = -1,         /** ...status is undefined */
        Idle,                   /** ...is idle */
        Running,                /** ...is currently running */
        RunningIndeterminate,   /** ...is currently running, but it's operating time is not known */
        Finished,               /** ...has finished successfully */
        AboutToBeAborted,       /** ...is about to be aborted */
        Aborting,               /** ...is in the process of being aborted */
        Aborted                 /** ...has been aborted */
    };

    /** Couples status enum value to status name string */
    static QMap<Status, QString> statusNames;

    /** Progress is tracked by: */
    enum class ProgressMode {
        Manual,     /** ...setting progress percentage manually */
        Subtasks,   /** ...setting a number of subtasks and flagging subtasks as finished (progress percentage is computed automatically) */
        Aggregate   /** ...combining the progress of child tasks */
    };

    /** Couples scope enum value to scope name string */
    static QMap<ProgressMode, QString> progressModeNames;

    /** Task: */
    enum class GuiScope {
        None,               /** ...has no explicit GUI handlers that show it in the gui (the task can still be observed in the tasks plugin) */
        Background,         /** ...will run in the background (not visible by default but can be revealed in the tasks view plugin) */
        Foreground,         /** ...will run in the foreground (tasks with this scope will automatically appear in a tasks popup when running) */
        Modal,              /** ...will run modally (tasks with this scope will automatically appear in a modal tasks dialog when the needed) */
        DataHierarchy,      /** ...will be displayed in the data hierarchy */
    };

    /** Couples scope enum value to scope name string */
    static QMap<GuiScope, QString> guiScopeNames;

    using TasksPtrs             = QVector<Task*>;
    using ProgressTextFormatter = std::function<QString(Task&)>;
    using GuiScopes             = QSet<GuiScope>;
    using Statuses              = QVector<Status>;

private:

    /** Timers for various purposes: */
    enum class TimerType {
        EmitProgressChanged,                /** For reducing the number of emissions of the Task::progressChanged() signal */
        EmitProgressDescriptionChanged,     /** For reducing the number of emissions of the Task::progressDescriptionChanged() signal */
        EmitProgressTextChanged,            /** For reducing the number of emissions of the Task::progressTextChanged() signal */
        DeferredStatus,                     /** To set task to status deferred */

        Count
    };

public:

    /**
     * Construct task with \p parent object, \p name, initial \p status, whether the task may be killed \p mayKill and possibly a \p taskHandler
     * 
     * @param parent Pointer to parent object (simply denotes the position of the task in the QObject hierarchy and has nothing to do with the task hierarchy)
     * @param name Name of the task
     * @param guiScopes GUI scopes of the task
     * @param status Initial status of the task
     * @param mayKill Boolean determining whether the task may be killed or not
     * @param handler Pointer to task handler
     */
    Task(QObject* parent, const QString& name, const GuiScopes& guiScopes = { GuiScope::None }, const Status& status = Status::Undefined, bool mayKill = false, AbstractTaskHandler* handler = nullptr);

    /** Remove from task manager when destructed */
    ~Task();

    /**
     * Get type (class) name (using QMetaObject)
     * @param humanFriendly Whether to include the task namespace or not
     * @return Task type name
     */
    QString getTypeName(bool humanFriendly = true) const;

public: // Configuration flags

    /**
     * Get configuration
     * @return Configuration
     */
    std::int32_t getConfiguration() const;

    /**
     * Check whether \p configurationFlag is set or not
     * @param configurationFlag Configuration flag
     * @return Boolean determining whether \p configurationFlag is set or not
     */
    bool isConfigurationFlagSet(ConfigurationFlag configurationFlag) const;

    /**
     * Set configuration flag
     * @param configurationFlag Configuration flag to set
     * @param unset Whether to unset the \p configurationFlag flag
     * @param recursive Whether to recursively set child configuration flag
     */
    void setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset = false, bool recursive = false);

    /**
     * Set configuration
     * @param configuration Configuration value
     * @param recursive Whether to recursively set child configuration flag
     */
    void setConfiguration(std::int32_t configuration, bool recursive = false);

public: // Weight

    /** 
     * Get weight
     * @return Get relative size of the progress interval this task occupies when it is a child task
     */
    virtual float getWeight() const;

    /**
     * Set weight
     * @param weight Relative size of the progress interval this task occupies when it is a child task
     */
    virtual void setWeight(float weight);

public: // Parent-child

    /**
     * Get parent task (if it has one)
     * @return Pointer to parent task if it has, nullptr otherwise
     */
    Task* getParentTask();

    /**
     * Set parent task to \p parentTask
     * @param parentTask Pointer to parent task
     */
    void setParentTask(Task* parentTask);

    /**
     * Determine whether the task has a parent task
     * @return Boolean determining whether the task has a parent task
     */
    bool hasParentTask();

    /**
     * Get child tasks
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @return Vector of pointers to child tasks
     */
    TasksPtrs getChildTasks(bool recursively = false, bool enabledOnly = true) const;

    /**
     * Get child tasks for \p statuses
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param status Filter on task status
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForStatuses(bool recursively = false, bool enabledOnly = true, const Statuses& statuses = Statuses()) const;

    /**
     * Get child tasks for \p guiScopes
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param guiScopes Filter on task GUI scope
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForGuiScopes(bool recursively = false, bool enabledOnly = true, const GuiScopes& guiScopes = GuiScopes()) const;

    /**
     * Get child tasks for \p guiScopes and \p statuses 
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param guiScopes Filter on task GUI scope
     * @param statuses Filter on task status
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForGuiScopesAndStatuses(bool recursively = false, bool enabledOnly = true, const GuiScopes& guiScopes = GuiScopes(), const Statuses& statuses = Statuses()) const;

protected: // Parent-child

    /**
     * Adds \p childTask to children
     * @param childTask Pointer to child task to add
     */
    virtual void addChildTask(Task* childTask);

    /**
     * Removes \p childTask from children
     * @param childTask Pointer to child task to remove
     */
    virtual void removeChildTask(Task* childTask);

public: // Name, description, icon and may kill

    /**
     * Get task name
     * @return Task name
     */
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

    /** Gets the task icon */
    QIcon getIcon() const;

    /**
     * Sets the task icon to \p icon
     * @param icon Task icon
     */
    void setIcon(const QIcon& icon);

    /**
     * Get whether the task is enabled or not
     * @return Boolean determining whether the task is enabled or not
     */
    bool getEnabled() const;

    /**
     * Sets whether the task is enabled or not
     * @param enabled Boolean determining whether the task is enabled or not
     * @param recursive Whether to set all descendants to \p enabled as well
     */
    void setEnabled(bool enabled, bool recursive = false);

    /**
     * Get whether the task is visible or not
     * @return Boolean determining whether the task is visible or not
     */
    bool getVisible() const;

    /**
     * Sets whether the task is visible or not
     * @param visible Boolean determining whether the task is visible or not
     */
    void setVisible(bool visible);

    /**
     * Get whether the task may be killed or not
     * @return Whether the task may be killed or not
     */
    bool getMayKill() const;

    /**
     * Sets whether the task may be killed or not
     * @param mayKill Boolean determining whether the task may be killed or not
     * @param recursive Whether to set all descendants to \p mayKill as well
     */
    void setMayKill(bool mayKill, bool recursive = false);

    /**
     * Get whether the task is killable
     * Return true when the following two criteria are met:
     *  - Task#_status is either Task::Status::Running or Task::Status::RunningIndeterminate
     *  - Task#_mayKill is true
     * @return Whether the task is killable
     */
    bool isKillable() const;

    /**
     * Resets the task internals
     * @param recursive Whether to reset all descendants as well
     */
    void reset(bool recursive = false);

    /**
     * Envoke Qt's processEvents on updates
     * @param alwaysProcessEvents Whether to invoke Qt's processEvents on updates
     */
    void setAlwaysProcessEvents(bool alwaysProcessEvents);

    /**
     * Get whether the task envokes Qt processEvents on updates
     * @return Whether the task envokes Qt's processEvents on updates
     */
    bool getAlwaysProcessEvents() const;

public: // Status

    /** Get task status */
    Status getStatus() const;

    /** Check if task is idle */
    bool isIdle() const;

    /** Check if task is running */
    bool isRunning() const;

    /** Check if task is running indeterminate */
    bool isRunningIndeterminate() const;

    /** Check if task is finished */
    bool isFinished() const;

    /** Check if task is about to be aborted */
    bool isAboutToBeAborted() const;

    /** Check if task is being aborted */
    bool isAborting() const;

    /** Check if task is aborted */
    bool isAborted() const;

    /**
     * Set task status to \p status, possibly \p recursive
     * @param status Task status
     * @param recursive Whether to set all descendants statuses as well
     */
    void setStatus(const Status& status, bool recursive = false);

    /**
     * Set task status deferred to \p status, possibly \p recursively and after \p delay
     * @param status Deferred task status
     * @param recursive Whether to set all descendants statuses as well
     * @param delay Delay after which the deferred task status is set
     */
    void setStatusDeferred(const Status& status, bool recursive = false, std::uint32_t delay = DEFERRED_TASK_STATUS_INTERVAL);

    /** Convenience method to set task status to undefined */
    void setUndefined();

    /** Convenience method to set task status to idle */
    void setIdle();

    /** Convenience method to set task status to running */
    void setRunning();

    /** Convenience method to set task status to running indeterminate */
    void setRunningIndeterminate();

    /** Convenience method to set task status to finished and use a custom progress description */
    void setFinished();

    /** Convenience method to set task status to about to be aborted */
    void setAboutToBeAborted();

    /** Convenience method to set task status to aborting */
    void setAborting();

    /** Convenience method to set task status to aborted */
    void setAborted();

    /**
     * Kill the task and trigger Task::abort() signal
     * @param recursive Boolean determining whether to also kill chill tasks recursively
     */
    void kill(bool recursive = true);

public: // Handler

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

public: // Progress mode

    /**
     * Get progress mode
     * @return Progress mode enum
     */
    ProgressMode getProgressMode() const;

    /**
     * Sets progress mode to to \p progressMode
     * @param progressMode Progress mode
     */
    void setProgressMode(const ProgressMode& progressMode);

public: // GUI scopes

    /**
     * Get GUI scopes
     * @return GUI scopes
     */
    GuiScopes getGuiScopes() const;
    
    /**
     * Sets GUI scopes to \p guiScopes
     * @param guiScopes GUI scope enum
     */
    void setGuiScopes(const GuiScopes& guiScopes);

    /**
     * Add \p guiScope to the GUI scopes set
     * @param guiScope GUI scope to add
     */
    void addGuiScope(const GuiScope& guiScope);

    /**
     * Remove \p guiScope from the GUI scopes set
     * @param guiScope GUI scope to remove
     */
    void removeGuiScope(const GuiScope& guiScope);

    /**
     * Function to establish whether at lease one GUI scope is present in both \p guiScopesA and \p guiScopesB
     * @param guiScopesA GUI scopes A
     * @param guiScopesB GUI scopes B
     */
    bool doGuiScopesOverlap(const GuiScopes& guiScopesA, const GuiScopes& guiScopesB) const;

    /**
     * Convert \p guiScopes to string list
     * @return String list of gui scopes
     */
    static QStringList guiScopesToStringlist(const GuiScopes& guiScopes);

public: // Manual

    /** Gets the task progress [0, 1] */
    float getProgress() const;

    /**
     * Sets the task progress percentage to \p progress
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Manual
     * @param progress Task progress, clamped to [0, 1]
     * @param subtaskDescription Subtask description associated with the progress update
     */
    virtual void setProgress(float progress, const QString& subtaskDescription = "");

    /**
     * Re-sets the task progress percentage to zero
     * @param recursive Boolean determining whether to also set the descendant tasks
     */
    virtual void resetProgress(bool recursive = false);

public: // Timers

    /**
     * Get timer by \p timerType
     * @return Timer for \p timerType
     */
    QTimer& getTimer(const TimerType& timerType);

    /**
     * Set timer \p interval for \p timerType
     * @param timerType Type of timer to set \p interval for
     * @param interval Interval for \p timerType
     */
    void setTimerInterval(const TimerType& timerType, std::uint32_t interval);

public: // Subtasks

    /**
     * Initializes the subtasks with \p numberOfSubtasks
     * This method sets the Task#_progressMode to ProgressMode::Subtasks
     * @param numberOfSubtasks Number of subtasks
     */
    void setSubtasks(std::uint32_t numberOfSubtasks);

    /**
     * Initializes the subtasks with \p subtasksNames
     * This method sets the Task#_progressMode to ProgressMode::Subtasks
     * @param subtasksNames Subtasks names
     */
    void setSubtasks(const QStringList& subtasksNames);

    /**
     * Flag item with \p subtaskIndex as started
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Index of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    void setSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription = QString());

    /**
     * Flag item with \p subtaskName as started
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskName Name of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    void setSubtaskStarted(const QString& subtaskName, const QString& progressDescription = QString());

    /**
     * Flag item with \p subtaskIndex as finished, the progress percentage will be computed automatically
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Index of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    void setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription = QString());

    /**
     * Flag item with \p subtaskName as finished, the progress percentage will be computed automatically
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * If \p subtaskName is not found, the progress will not be updated (ensure that subtasks names are set with Task::setSubtasksNames() or Task::setSubtasksName())
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskName Name of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    void setSubtaskFinished(const QString& subtaskName, const QString& progressDescription = QString());

    /**
     * Set subtask name to \p subtaskName for \p subtaskIndex
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Subtask index to set the description for
     * @param subtaskName Name of the subtask
     */
    void setSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName);

    /**
     * Get subtask names
     * @return Subtasks names
     */
    QStringList getSubtasksNames() const;

    /**
     * Get subtask name for \p subtaskIndex
     * @param subtaskIndex Index of the subtask
     * @return Subtasks name for \p subtaskIndex, empty string if not found
     */
    QString getSubtasksName(std::uint32_t subtaskIndex) const;

    /**
     * Get subtask index for \p subtaskName
     * Returns -1 when \p subtaskName is not found or Task#_progressMode is set to ProgressMode::Manual
     * @param subtaskName Name of the subtask
     */
    std::int32_t getSubtaskIndex(const QString& subtaskName) const;

    /**
     * Get subtask name prefix
     * @return String to prefix unnamed subtasks with
     */
    QString getSubtaskNamePrefix() const;

    /**
     * Set subtask name prefix to \p subtaskNamePrefix
     * @param subtaskNamePrefix String to prefix unnamed subtasks with
     */
    void setSubtaskNamePrefix(const QString& subtaskNamePrefix);

public: // Progress description

    /**
     * Get progress description
     * @return Progress description
     */
    QString getProgressDescription() const;

    /**
     * Set progress description to \p progressDescription
     * @param progressDescription Progress description
     * @param clearDelay Progress description will be cleared after this delay (do not clear if zero)
     */
    void setProgressDescription(const QString& progressDescription, std::uint32_t clearDelay = 0);

public: // Progress text

    /** Gets the task progress text */
    QString getProgressText() const;

    /**
     * Set progress text formatter to \p progressTextFormatter
     * @param progressTextFormatter Progress formatter function
     */
    void setProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter);

private:

    /** Gets the standard task progress text */
    QString getStandardProgressText() const;

private:

    /** Updates the progress percentage depending on the task settings */
    void updateProgress();

    /** Updates the progress text depending on the task settings */
    void updateProgressText();

private: // For aggregate task

    /**
     * Registers \p childTask
     * @param childTask Pointer to child task to register
     */
    void registerChildTask(Task* childTask);

    /**
     * Unregisters \p childTask
     * @param childTask Pointer to child task to unregister
     */
    void unregisterChildTask(Task* childTask);

    /**
     * Update status of aggregate task
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Aggregate
     */
    void updateAggregateStatus();

private: // Private setters (these call private signals under the hood, an essential part to make cross thread task usage possible)

    void privateSetParentTask(Task* parentTask);
    void privateAddChildTask(Task* childTask);
    void privateRemoveChildTask(Task* childTask);
    void privateSetName(const QString& name);
    void privateSetDescription(const QString& description);
    void privateSetIcon(const QIcon& icon);
    void privateSetEnabled(bool enabled, bool recursive = false);
    void privateSetVisible(bool visible);
    void privateSetMayKill(bool mayKill, bool recursive = false);
    void privateReset(bool recursive = false);
    void privateSetStatus(const Status& status, bool recursive = false);
    void privateSetStatusDeferred(const Status& status, bool recursive = false, std::uint32_t delay = DEFERRED_TASK_STATUS_INTERVAL);
    void privateSetUndefined();
    void privateSetIdle();
    void privateSetRunning();
    void privateSetRunningIndeterminate();
    void privateSetFinished();
    void privateSetAboutToBeAborted();
    void privateSetAborting();
    void privateSetAborted();
    void privateKill(bool recursive = true);
    void privateSetProgressMode(const ProgressMode& progressMode);
    void privateSetGuiScopes(const GuiScopes& guiScopes);
    void privateAddGuiScope(const GuiScope& guiScope);
    void privateRemoveGuiScope(const GuiScope& guiScope);
    void privateResetProgress(bool recursive = false);
    void privateSetProgress(float progress, const QString& subtaskDescription = "");
    void privateSetSubtasks(std::uint32_t numberOfSubtasks);
    void privateSetSubtasks(const QStringList& subtasksNames);
    void privateSetSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription);
    void privateSetSubtaskStarted(const QString& subtaskName, const QString& progressDescription);
    void privateSetSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription);
    void privateSetSubtaskFinished(const QString& subtaskName, const QString& progressDescription);
    void privateSetSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName);
    void privateSetProgressDescription(const QString& progressDescription, std::uint32_t clearDelay = 0);
    void privateSetProgressText(const QString& progressText, std::uint32_t clearDelay = 0);
    void privateSetProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter);

private:

    void privateEmitProgressChanged();
    void privateEmitProgressDescriptionChanged();
    void privateEmitProgressTextChanged();

public: // Advanced (only use methods if consequences are understood)

    /**
     * Adds this task to the task manager
     * Under normal circumstances, tasks are automatically added to the task manager during construction.
     * In some cases, the task manager is not yet present then and therefore this method allows to
     * manually add this task to the task manager at a later point in time.
     * Only use this in edge cases!
     */
    void addToTaskManager();

private: // Task manager

    /** Add this task to the task manager */
    void privateAddToTaskManager();

signals:

    /**
     * Signals that the task name changed to \p name
     * @param name Modified name
     */
    void nameChanged(const QString& name);

    /**
     * Signals that \p configurationFlag is \p set
     * @param configurationFlag Toggled configuration flag
     * @param set Whether the flag was set or unset
     */
    void configurationFlagToggled(const ConfigurationFlag& configurationFlag, bool set);

    /**
     * Signals that the configuration changed
     * @param configuration New configuration
     */
    void configurationChanged(std::int32_t configuration);

    /**
     * Signals that the task description changed to \p description
     * @param description Modified description
     */
    void descriptionChanged(const QString& description);

    /**
     * Signals that the task icon changed to \p icon
     * @param icon Modified icon
     */
    void iconChanged(const QIcon& icon);

    /**
     * Signals that the task enabled state changed to \p enabled
     * @param enabled Whether the task is enabled
     */
    void enabledChanged(bool enabled);

    /**
     * Signals that the task visibility changed to \p visible
     * @param visible Whether the task is visible in the user interface
     */
    void visibileChanged(bool visible);

    /**
     * Signals that the task handler changed to \p handler
     * @param handler Task handler
     */
    void handlerChanged(AbstractTaskHandler* handler);

    /**
     * Signals that the task status changed from \p previousStatus to \p status
     * @param previousStatus Status before the modification
     * @param status Modified name
     */
    void statusChanged(const Status& previousStatus, const Status& status);

    /**
     * Signals that may kill changed to \p mayKill
     * @param mayKill Boolean determining whether the task may be killed or not
     */
    void mayKillChanged(bool mayKill);

    /**
     * Signals that killable changed to \p killable
     * @param killable Boolean determining whether the task is killable or not
     */
    void isKillableChanged(bool killable);

    /** Signals that the task status changed to undefined */
    void statusChangedToUndefined();

    /** Signals that the task became idle */
    void statusChangedToIdle();

    /** Signals that the task started running */
    void statusChangedToRunning();

    /** Signals that the task started running indeterminately */
    void statusChangedToRunningIndeterminate();

    /** Signals that the task finished */
    void statusChangedToFinished();

    /** Signals that the task is about to be aborted */
    void statusChangedToAboutToBeAborted();

    /** Signals that the task is aborting */
    void statusChangedToAborting();

    /** Signals that the task is aborted */
    void statusChangedToAborted();

    /** Requests the involved task algorithms to abort their work */
    void requestAbort();

    /**
     * Signals that the task progress mode changed to \p progressMode
     * @param progressMode Modified progress mode
     */
    void progressModeChanged(const ProgressMode& progressMode);

    /**
     * Signals that the task GUI scopes changed to \p guiScopes
     * @param guiScopes Modified GUI scopes
     */
    void guiScopesChanged(const GuiScopes& guiScopes);

    /**
     * Signals that the task progress changed to \p progress
     * @param progress Modified progress
     */
    void progressChanged(float progress);

    /**
     * Signals that subtasks with \p subtasksNames have been added
     * @param subtasksNames Added subtasks names
     */
    void subtasksAdded(const QStringList& subtasksNames);

    /**
     * Signals that subtasks changed to \p subtasks
     * @param subtasks Modified subtasks
     * @param subtasksNames Modified subtasks names
     */
    void subtasksChanged(const QBitArray& subtasks, const QStringList& subtasksNames);

    /**
     * Signals that subtask with \p name started
     * @param subTaskName Name of the subtask that started
     */
    void subtaskStarted(const QString& subTaskName);

    /**
     * Signals that subtask with \p subtaskIndex finished
     * @param subtaskIndex Index of the subtask that finished
     * @param subtaskName Name of the subtask that finished (set when Task#_subtasksNames is set)
     */
    void subtaskFinished(std::uint32_t subtaskIndex, const QString& subtaskName = "");

    /**
     * Signals that subtask name prefix changed from \p previousSubtaskNamePrefix to \p currentSubtaskNamePrefix
     * @param previousSubtaskNamePrefix Previous subtask name prefix
     * @param currentSubtaskNamePrefix Current subtask name prefix
     */
    void subtaskNamePrefixChanged(const QString& previousSubtaskNamePrefix, const QString& currentSubtaskNamePrefix);

    /**
     * Signals that the progress description changed to \p progressDescription
     * @param progressDescription Modified progress description
     */
    void progressDescriptionChanged(const QString& progressDescription);

    /**
     * Signals that the progress text changed to \p progressText
     * @param progressText Current progress text
     */
    void progressTextChanged(const QString& progressText);

    /**
     * Signals that the parent task changed from \p previousParentTask to \p currentParentTask
     * @param previousParentTask Pointer to previous parent task (maybe nullptr)
     * @param currentParentTask Pointer to previous parent task (maybe nullptr)
     */
    void parentTaskChanged(Task* previousParentTask, Task* currentParentTask);

    /**
     * Signals that \p childTask has been added
     * @param childTask Pointer to child task that was added
     */
    void childTaskAdded(Task* childTask);

    /**
     * Signals that \p childTask is about to be removed from the list of children
     * @param childTask Pointer to child task that is about to be removed from the list of children
     */
    void childTaskAboutToBeRemoved(Task* childTask);

    /**
     * Signals that \p childTask has been removed from the list of children
     * @param childTask Pointer to child task that has been removed from the list of children
     */
    void childTaskRemoved(Task* childTask);

    /**
     * The signals below are private signals and can/should only be called from within this Task class
     * These signals provide a way to make cross thread task usage possible
     */
    void privateSetParentTaskSignal(Task* parentTask, QPrivateSignal);
    void privateAddChildTaskSignal(Task* childTask, QPrivateSignal);
    void privateRemoveChildTaskSignal(Task* childTask, QPrivateSignal);
    void privateSetNameSignal(const QString& name, QPrivateSignal);
    void privateSetDescriptionSignal(const QString& description, QPrivateSignal);
    void privateSetIconSignal(const QIcon& icon, QPrivateSignal);
    void privateSetEnabledSignal(bool enabled, bool recursive, QPrivateSignal);
    void privateSetVisibleSignal(bool visible, QPrivateSignal);
    void privateSetMayKillSignal(bool mayKill, bool recursive, QPrivateSignal);
    void privateResetSignal(bool recursive, QPrivateSignal);
    void privateSetStatusSignal(const Status& status, bool recursive, QPrivateSignal);
    void privateSetStatusDeferredSignal(const Status& status, bool recursive, std::uint32_t delay, QPrivateSignal);
    void privateSetUndefinedSignal(QPrivateSignal);
    void privateSetIdleSignal(QPrivateSignal);
    void privateSetRunningSignal(QPrivateSignal);
    void privateSetRunningIndeterminateSignal(QPrivateSignal);
    void privateSetFinishedSignal(QPrivateSignal);
    void privateSetAboutToBeAbortedSignal(QPrivateSignal);
    void privateSetAbortingSignal(QPrivateSignal);
    void privateSetAbortedSignal(QPrivateSignal);
    void privateKillSignal(bool, QPrivateSignal);
    void privateSetProgressModeSignal(const ProgressMode& progressMode, QPrivateSignal);
    void privateSetGuiScopesSignal(const GuiScopes& guiScopes, QPrivateSignal);
    void privateAddGuiScopeSignal(const GuiScope& guiScope, QPrivateSignal);
    void privateRemoveGuiScopeSignal(const GuiScope& guiScope, QPrivateSignal);
    void privateResetProgressSignal(bool recursive, QPrivateSignal);
    void privateSetProgressSignal(float progress, const QString& subtaskDescription, QPrivateSignal);
    void privateSetSubtasksSignal(std::uint32_t numberOfSubtasks, QPrivateSignal);
    void privateSetSubtasksSignal(const QStringList& subtasksNames, QPrivateSignal);
    void privateSetSubtaskStartedSignal(std::uint32_t subtaskIndex, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskStartedSignal(const QString& subtaskName, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskFinishedSignal(std::uint32_t subtaskIndex, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskFinishedSignal(const QString& subtaskName, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskNameSignal(std::uint32_t subtaskIndex, const QString& subtaskName, QPrivateSignal);
    void privateSetProgressDescriptionSignal(const QString& progressDescription, std::uint32_t clearDelay, QPrivateSignal);
    void privateSetProgressTextFormatterSignal(const ProgressTextFormatter& progressTextFormatter, QPrivateSignal);

private:
    std::int32_t            _configuration;                                 /** Configuration flags */
    float                   _weight;                                        /** Relative size of the progress interval this task occupies when it is a child task */
    QString                 _name;                                          /** Task name */
    QString                 _description;                                   /** Task description */
    QIcon                   _icon;                                          /** Task icon */
    bool                    _enabled;                                       /** Whether the task is enabled, disabled tasks are not included in task aggregation */
    bool                    _visible;                                       /** Whether the task is visible in the user interface */
    Status                  _status;                                        /** Task status */
    Status                  _deferredStatus;                                /** Task status which is set after a delay */
    bool                    _deferredStatusRecursive;                       /** Whether to set the task status deferred recursively */
    bool                    _mayKill;                                       /** Whether the task may be killed or not */
    bool                    _alwaysProcessEvents;                           /** Whether the task envokes Qt's processEvents on updates, e.g. for task before the main event loop started. Default: false */
    AbstractTaskHandler*    _handler;                                       /** Task handler */
    ProgressMode            _progressMode;                                  /** The way progress is recorded */
    GuiScopes               _guiScopes;                                     /** The gui scope(s) in which the task will present itself to the user */
    float                   _progress;                                      /** Task progress */
    QTimer                  _timers[static_cast<int>(TimerType::Count)];    /** Timers to prevent unnecessary abundant emissions of various signals */
    QBitArray               _subtasks;                                      /** Subtasks status */
    QStringList             _subtasksNames;                                 /** Subtasks names */
    QString                 _subtaskNamePrefix;                             /** String to prefix unnamed subtasks with */
    QString                 _progressDescription;                           /** Current item description */
    Task*                   _parentTask;                                    /** Pointer to the parent task */
    TasksPtrs               _childTasks;                                    /** Pointers to child tasks */
    QString                 _progressText;                                  /** Progress text */
    ProgressTextFormatter   _progressTextFormatter;                         /** Progress text formatter function (overrides Task::getProgressText() when set) */

private:
    static constexpr std::uint32_t EMIT_CHANGED_TIMER_INTERVAL      = 100;      /** Single shot task progress and description timer interval */
    static constexpr std::uint32_t DEFERRED_TASK_STATUS_INTERVAL    = 1500;     /** Delay after which the deferred task status is set */
};

}
