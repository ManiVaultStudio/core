#pragma once

#include "AbstractManager.h"

#include <QObject>

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

    //using CreateActionFunction = std::function<WidgetAction*(const hdps::Datasets&)>;

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
     * Get all actions in the manager
     * @return List of all actions in the manager
     */
    virtual const gui::WidgetActions& getActions() const = 0;

    /**
     * Get actions with \p id
     * @return Pointer to widget action (may return nullptr)
     */
    virtual gui::WidgetAction* getAction(const QString& id) = 0;

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
     * Add public action to the manager
     * @param action Pointer to public action
     */
    virtual void addPublicAction(gui::WidgetAction* action) = 0;

    /**
     * Remove public action from the manager
     * @param action Pointer to public action
     */
    virtual void removePublicAction(gui::WidgetAction* action) = 0;

protected:

    /**
     * Make widget \p action public
     * @param action Pointer to action
     */
    virtual void makeActionPublic(gui::WidgetAction* action) final {
        Q_ASSERT(action != nullptr);

        action->makePublic();
    }

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

    /**
     * Signals that public \p action is added to the manager
     * @param action Pointer to public action that is added to the manager
     */
    void publicActionAdded(gui::WidgetAction* action);

    /**
     * Signals that public \p action is about to be removed from the manager
     * @param action Pointer to public action that is about to be removed from the manager
     */
    void publicActionAboutToBeRemoved(gui::WidgetAction* action);

    /**
     * Signals that public action with \p actionId was removed from the manager
     * @param actionId Globally unique identifier of the public action that was removed from the manager
     */
    void publicActionRemoved(const QString& actionId);
};

}
