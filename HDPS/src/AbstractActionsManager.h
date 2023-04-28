#pragma once

#include "AbstractManager.h"

#include "models/ActionsModel.h"
#include "models/ActionsFilterModel.h"

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

public: // Linking

    /**
     * Publish \p privateAction so that other private actions can connect to it
     * @param privateAction Pointer to private action to publish
     * @param name Name of the published widget action (if empty, a name choosing dialog will popup)
     * @param recursive Whether to also publish the child actions recursively
     */
    virtual void publishPrivateAction(gui::WidgetAction* privateAction, const QString& name = "", bool recursive = true) = 0;

    /**
     * Connect \p privateAction to \p publicAction
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    virtual void connectPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        privateAction->_publicAction = publicAction;

        privateAction->connectToPublicAction(publicAction);
    }

    /**
     * Disconnect \p privateAction from public action
     * @param privateAction Pointer to private action
     */
    virtual void disconnectPrivateActionFromPublicAction(gui::WidgetAction* privateAction) final {
        Q_ASSERT(privateAction != nullptr);

        if (privateAction == nullptr)
            return;

        if (privateAction->isConnected())
            privateAction->disconnectFromPublicAction();

        privateAction->_publicAction = nullptr;
    }

protected:

    /**
     * Add \p privateAction to the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    void addPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        publicAction->getConnectedActions() << privateAction;

        emit publicAction->actionConnected(privateAction);

        connect(privateAction, &gui::WidgetAction::destroyed, this, [&]() -> void {
            removePrivateActionFromPublicAction(privateAction, publicAction);
        });
    }

    /**
     * Remove \p privateAction from the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    void removePrivateActionFromPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        publicAction->getConnectedActions().removeOne(privateAction);

        emit publicAction->actionDisconnected(privateAction);
    }

public: // Model

    /**
     * Get model which contains all actions
     * @return Reference to actions model
     */
    virtual ActionsModel& getModel() = 0;

protected:

    /**
     * Add \p actionType
     * @param actionType Action type to add
     */
    void addActionType(const QString& actionType) {
        const auto cachedActionTypes = getActionTypes();

        if (_actionTypes.contains(actionType)) {
            _actionTypes[actionType]++;
        } else {
            _actionTypes[actionType] = 1;

            emit actionTypeAdded(actionType);
        }

        if (getActionTypes() != cachedActionTypes)
            emit actionTypesChanged(getActionTypes());
    }

    /**
     * Remove \p actionType
     * @param actionType Action type to remove
     */
    void removeActionType(const QString& actionType) {
        const auto cachedActionTypes = getActionTypes();

        if (!_actionTypes.contains(actionType))
            return;

        _actionTypes[actionType]--;

        if (_actionTypes[actionType] <= 0) {
            _actionTypes.remove(actionType);

            emit actionTypeRemoved(actionType);
        }

        if (getActionTypes() != cachedActionTypes)
            emit actionTypesChanged(getActionTypes());
    }

public:

    /**
     * Get set of action types
     * @return List action types
     */
    const QStringList getActionTypes() const {
        auto actionTypes = _actionTypes.keys();

        actionTypes.removeDuplicates();

        return actionTypes;
    }

protected:

    /**
     * Make widget \p action public
     * @param action Pointer to action
     */
    virtual void makeActionPublic(gui::WidgetAction* action) final {
        Q_ASSERT(action != nullptr);

        if (action == nullptr)
            return;

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
     * Signals that a new \p actionType has been added to the manager
     * @param actionType Action type
     */
    void actionTypeAdded(const QString& actionType);

    /**
     * Signals that the last instance of \p actionType has been removed from the manager
     * @param actionType Action type
     */
    void actionTypeRemoved(const QString& actionType);

    /**
     * Signals that the \p actionTypes changed
     * @param actionTypes Action types
     */
    void actionTypesChanged(const QStringList& actionTypes);

    friend class gui::WidgetAction;

protected:
    gui::WidgetActions          _actions;       /** Flat list of actions that are instantiated in the plugin system */
    QMap<QString, std::int32_t> _actionTypes;   /** Number of rows per action type */
};

}
