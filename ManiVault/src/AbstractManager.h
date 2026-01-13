// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include "actions/LockingAction.h"

#include <QObject>
#include <QDebug>
#include <QIcon>

#ifdef _DEBUG
    //#define ABSTRACT_MANAGER_VERBOSE
#endif

namespace mv
{

/**
 * Abstract manager
 * 
 * Base abstract manager class for the core.
 *
 * @ingroup mv_managers
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractManager : public QObject, public util::Serializable
{
    Q_OBJECT

public:

    /**
     * Construct manager with pointer to \p parent object and \p name
     * @param parent Pointer to parent object
     * @param name Manager name (serialization name)
     */
    AbstractManager(QObject* parent, const QString& name = "") :
        QObject(parent),
        Serializable(name),
        _initialized(false),
        _lockingAction(nullptr),
        _coreIsDestroyed(false),
        _isInitializing(false),
        _isResetting(false)
    {
    }

    /** Perform manager startup initialization */
    virtual void initialize() {
        _lockingAction = new gui::LockingAction(this, getSerializationName());
    };

    /** Begin reset operation */
    void beginReset() {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        emit managerAboutToBeReset();

        _isResetting = true;
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
    void endReset() {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        _isResetting = false;

        emit managerReset();
    }

    /** Begin the initialization process */
    void beginInitialization() {
#ifdef ABSTRACT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        _isInitializing = true;

        if (isInitialized()) {
            qDebug() << getSerializationName() << "already initialized";
        }
        else {
            emit managerAboutToBeInitialized();
        }
    }

    /** End the initialization process */
    void endInitialization() {
#ifdef ABSTRACT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        _initialized    = true;
        _isInitializing = false;

        emit managerInitialized();
    }

    /**
     * Get whether the manager is initialized or not
     * @return Boolean determining whether the manager is initialized or not
     */
    bool isInitialized() const {
        return _initialized;
    }

    /**
     * Get whether the manager is initializing or not
     * @return Boolean determining whether the manager is initializing or not
     */
    bool isInitializing() const {
        return _isInitializing;
    }

    /**
     * Get whether the manager is resetting or not
     * @return Boolean determining whether the manager is resetting or not
     */
    bool isResetting() const {
        return _isResetting;
    }

    /**
     * Get manager icon
     * @return Icon
     */
    virtual QIcon getIcon() const {
        return QIcon();
    }

public:

    /**
     * Get manager settings prefix
     * @return Settings manager prefix
     */
    QString getSettingsPrefix() const {
        return QString("Managers/%1/").arg(getSerializationName());
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

protected: // Core destruction

    /** Mark core as destroyed */
    void setCoreIsDestroyed() {
        _coreIsDestroyed = true;
    }

public: // Core destruction

    /**
     * Establish whether the core is destroyed
     * @return Boolean determining whether the core is destroyed
     */
    bool isCoreDestroyed() const {
        return _coreIsDestroyed;
    }

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
    bool                    _coreIsDestroyed;   /** Boolean determining whether the core is destroyed */
    bool                    _isInitializing;    /** Boolean determining whether the manager is being initialized */
    bool                    _isResetting;       /** Boolean determining whether the manager is resetting */

    friend class Core;
};

}
