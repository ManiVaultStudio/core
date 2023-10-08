// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include "actions/LockingAction.h"

#include "Task.h"

#include <QObject>
#include <QDebug>
#include <QIcon>

#ifdef _DEBUG
    //#define ABSTRACT_MANAGER_VERBOSE
#endif

namespace hdps
{

/**
 * Abstract manager
 * 
 * Base abstract manager class for the core.
 * 
 * @author Thomas Kroes
 */
class AbstractManager : public QObject, public util::Serializable
{
    Q_OBJECT

public:

    /** Subtask type */
    enum class SubtaskType {
        Initialize,
        Reset
    };

    /**
     * Get subtask name for \p subtaskType
     * @param subtaskType Type of subtask
     */
    QString getSubtaskName(const SubtaskType& subtaskType) const {
        switch (subtaskType) {
            case SubtaskType::Initialize:
                return QString("Initialize %1").arg(getSerializationName());

            case SubtaskType::Reset:
                return QString("Reset %1").arg(getSerializationName());

            default:
                break;
        }

        return "";
    }

public:

    /**
     * Construct manager with \p parent object and \p name
     * @param parent Pointer to parent object
     * @param name Manager name (serialization name)
     */
    AbstractManager(QObject* parent = nullptr, const QString& name = "") :
        QObject(parent),
        Serializable(name),
        _initialized(false),
        _lockingAction(nullptr),
        _task(nullptr)
    {
    }

    /** Perform manager startup initialization */
    virtual void initialize() {
        _lockingAction = new gui::LockingAction(this, getSerializationName());
    };

    /** Create task for reporting progress during initialization and reset operations */
    virtual void createTask() final {
        if (_task == nullptr)
            _task = new Task(this, getSerializationName());
    }

    /** Begin reset operation */
    virtual void beginReset() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        emit managerAboutToBeReset();

        _task->setSubtasks({ getSubtaskName(SubtaskType::Reset) });
        _task->setRunning();
        _task->setSubtaskStarted({ getSubtaskName(SubtaskType::Reset) });
    }

    /** Resets the contents of the manager */
    virtual void reset() {
#ifdef ABSTRACT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        if (!isInitialized())
            qDebug() << getSerializationName() << "not initialized";
    }

    /** End reset operation */
    virtual void endReset() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        emit managerReset();

        _task->setSubtaskFinished({ getSubtaskName(SubtaskType::Reset) });
        _task->setFinished();
    }

    /** Begin the initialization process */
    virtual void beginInitialization() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        if (isInitialized()) {
            qDebug() << getSerializationName() << "already initialized";
        }
        else {
            emit managerAboutToBeInitialized();

            _task->setSubtasks({ getSubtaskName(SubtaskType::Initialize) });
            _task->setRunning();
            _task->setSubtaskStarted({ getSubtaskName(SubtaskType::Initialize) });
        }
    }

    /** End the initialization process */
    virtual void endInitialization() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        _initialized = true;

        emit managerInitialized();

        _task->setSubtaskFinished({ getSubtaskName(SubtaskType::Initialize) });
        _task->setFinished();
    }

    /**
     * Get whether the manager is initialized or not
     * @return Boolean determining whether the manager is initialized or not
     */
    virtual bool isInitialized() const final {
        return _initialized;
    }

    /**
     * Get manager icon
     * @return Icon
     */
    virtual QIcon getIcon() const {
        return QIcon();
    }

    /**
     * Get task
     * @return Pointer to task for reporting progress during initialization and reset operations
     */
    virtual Task* getTask() final {
        return _task;
    }

public: // Action getters

    gui::LockingAction& getLockingAction() { return *_lockingAction; }

public: // Locking

    /**
     * Get whether the manager be locked
     * @return Boolean determining whether the manager be locked
     */
    virtual bool mayLock() const {
        return true;
    };

    /**
     * Get whether the manager be unlocked
     * @return Boolean determining whether the manager be unlocked
     */
    virtual bool mayUnlock() const {
        return true;
    };
    
signals:

    /** Signals that the initialization process has begun */
    void managerAboutToBeInitialized();

    /** Signals that the initialization process has ended */
    void managerInitialized();

    /** Signals that the manager is about to be reset */
    void managerAboutToBeReset();

    /** Signals that the manager has been reset */
    void managerReset();

private:
    bool                    _initialized;       /** Whether the manager is initialized or not */
    gui::LockingAction*     _lockingAction;     /** Manager locking action */
    Task*                   _task;              /** Task for reporting progress during initialization and reset operations */
};

}
