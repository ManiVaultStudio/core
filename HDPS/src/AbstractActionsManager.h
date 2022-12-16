#pragma once

#include "AbstractManager.h"

#include <QObject>
#include <QAbstractItemModel>

namespace hdps
{

namespace gui {
    class WidgetAction;
}

/**
 * Abstract actions manager
 *
 * Base abstract actions manager class for managing widget actions.
 *
 * @author Thomas Kroes
 */
class AbstractActionsManager : public AbstractManager
{
    Q_OBJECT

public:

    /**
     * Construct actions manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractActionsManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Actions")
    {
    }

    /**
     * Add action to the manager
     * @param action Pointer to action
     */
    virtual void addAction(gui::WidgetAction* action) = 0;

    /**
     * Remove action from the manager
     * @param action Pointer to action
     */
    virtual void removeAction(gui::WidgetAction* action) = 0;

    /**
     * Get model which is associated with the manager
     * @return Reference to an abstract item model inherited model class
     */
    virtual const QAbstractItemModel& getModel() const = 0;

signals:

    /**
     * Signals that \p action is added to the manager
     * @param action Pointer to action that is added to the manager
     */
    void actionAdded(gui::WidgetAction* action);

    /**
     * Signals that \p action is about to be removed from the manager
     * @param action Pointer to action that is about to be removed from the manager
     */
    void actionAboutToBeRemoved(gui::WidgetAction* action);

    /**
     * Signals that action with \p actionId was removed from the manager
     * @param actionId Globally unique identifier of the action that was removed from the manager
     */
    void actionRemoved(const QString& actionId);
};

}
