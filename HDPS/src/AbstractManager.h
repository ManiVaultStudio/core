#pragma once

#include "util/Serializable.h"

#include <QObject>
#include <QDebug>

#ifdef _DEBUG
    #define ABSTRACT_MANAGER_VERBOSE
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

    /**
     * Construct manager with \p parent object and \p name
     * @param parent Pointer to parent object
     * @param name Manager name (serialization name)
     */
    AbstractManager(QObject* parent = nullptr, const QString& name = "") :
        QObject(parent),
        Serializable(name)
    {
    }

    /** Perform manager startup initialization */
    virtual void initalize() = 0;

    /** Begin reset operation */
    virtual void beginReset() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        emit managerAboutToBeReset();
    }

    /** Resets the contents of the manager */
    virtual void reset() = 0;

    /** End reset operation */
    virtual void endReset() final {
#ifdef ABSTRACT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

        emit managerReset();
    }

signals:

    /** Signals that the manager is about to be reset */
    void managerAboutToBeReset();

    /** Signals that the manager has been reset */
    void managerReset();
};

}
