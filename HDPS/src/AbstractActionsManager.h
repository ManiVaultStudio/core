#ifndef ABSTRACT_ACTION_MANAGER_H
#define ABSTRACT_ACTION_MANAGER_H
#pragma once

#include "AbstractManager.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

#include <QObject>

namespace hdps
{

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

private:

    /** Number of instances for action type and human-friendly action type string */
    using ActionType = QPair<std::int32_t, QString>;

public:

    /**
     * Construct actions manager with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractActionsManager(QObject* parent = nullptr) :
        AbstractManager(parent, "Actions"),
        _actions(),
        _publicActions(),
        _actionTypes()
    {
    }

    /**
     * Get all actions in the manager
     * @return List of all actions in the manager
     */
    virtual const gui::WidgetActions& getActions() const final {
        return _actions;
    }

    /**
     * Get all public actions in the manager
     * @return List of all public actions in the manager
     */
    virtual const gui::WidgetActions& getPublicActions() const final {
        return _publicActions;
    }

    /**
     * Get action with \p id
     * @return Pointer to widget action (may return nullptr)
     */
    virtual gui::WidgetAction* getAction(const QString& id) final {
        for (const auto action : _actions)
            if (id == action->getId())
                return action;

        return nullptr;
    }

    /**
     * Add \p action of action type to the manager
     * @param action Pointer to action
     */
    template<typename ActionType>
    void addAction(ActionType* action) {
        Q_ASSERT(action != nullptr);

        _actions << action;

        emit actionAdded(action);

        connect(action, &gui::WidgetAction::scopeChanged, this, [this, action](const gui::WidgetAction::Scope& scope) -> void {
            switch (scope)
            {
                case gui::WidgetAction::Scope::Public: {
                    addPublicAction(action);
                    break;
                }
                
                case gui::WidgetAction::Scope::Private: {
                    removePublicAction(action);
                    break;
                }
            }
        });

        addActionType(action->getTypeString());
    }

    /**
     * Remove \p action of action type from the manager
     * @param action Pointer to action
     */
    template<typename ActionType>
    void removeAction(ActionType* action) {
        Q_ASSERT(action != nullptr);

        try
        {
            const auto actionId = action->getId();

            emit actionAboutToBeRemoved(action);
            {
                if (_actions.contains(action))
                    _actions.removeOne(action);
            }
            emit actionRemoved(actionId);

            if (action->isPublic())
                removePublicAction(action);

            removeActionType(action->getTypeString());
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to remove %1 from actions manager").arg(action->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to remove %1 from actions manager").arg(action->text()));
        }
    }

private: // Public actions

    /**
     * Add \p publicAction to the manager
     * @param publicAction Pointer to public action
     */
    virtual void addPublicAction(gui::WidgetAction* publicAction) final {
        Q_ASSERT(publicAction != nullptr);

        _publicActions << publicAction;

        emit publicActionAdded(publicAction);
    }

    /**
     * Remove \p publicAction from the manager
     * @param publicAction Pointer to public action
     */
    virtual void removePublicAction(gui::WidgetAction* publicAction) final {
        Q_ASSERT(publicAction != nullptr);

        if (!_publicActions.contains(publicAction))
            return;

        emit publicActionAboutToBeRemoved(publicAction);
        {
            for (auto connectedAction : publicAction->getConnectedActions())
                disconnectPrivateActionFromPublicAction(connectedAction, true);

            _publicActions.removeOne(publicAction);
        }
        emit publicActionRemoved(publicAction->getId());
    }

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
     * @param recursive Whether to also connect descendant child actions
     */
    virtual void connectPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction, bool recursive) final;

    /**
     * Connect \p privateSourceAction to \p privateTargetAction
     * @param privateSourceAction Pointer to private source action
     * @param privateTargetAction Pointer to private target action (private source action will be connected to published private target action)
     * @param publicActionName Name of the public action (ask for name if empty)
     */
    virtual void connectPrivateActions(gui::WidgetAction* privateSourceAction, gui::WidgetAction* privateTargetAction, const QString& publicActionName = "") final;

    /**
     * Disconnect \p privateAction from public action
     * @param privateAction Pointer to private action
     * @param recursive Whether to also disconnect descendant child actions
     */
    virtual void disconnectPrivateActionFromPublicAction(gui::WidgetAction* privateAction, bool recursive) final;

protected:

    /**
     * Add \p privateAction to the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    virtual void addPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) final {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        try
        {
            publicAction->getConnectedActions() << privateAction;

            emit publicAction->actionConnected(privateAction);

            connect(privateAction, &gui::WidgetAction::destroyed, this, [this, privateAction, publicAction]() -> void {
                removePrivateActionFromPublicAction(privateAction, publicAction);
                });
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to add %1 to %2:").arg(privateAction->text(), publicAction->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to add %1 to %2:").arg(privateAction->text(), publicAction->text()));
        }
    }

    /**
     * Remove \p privateAction from the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    virtual void removePrivateActionFromPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) final {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        try
        {
            publicAction->getConnectedActions().removeOne(privateAction);

            emit publicAction->actionDisconnected(privateAction);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to remove %1 from %2:").arg(privateAction->text(), publicAction->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to remove %1 from %2:").arg(privateAction->text(), publicAction->text()));
        }
    }

protected: // Action types

    /**
     * Add \p actionType string
     * @param actionType Action type string to add
     */
    void addActionType(const QString& actionType) {
        const auto cachedActionTypes = getActionTypes();

        if (_actionTypes.contains(actionType)) {
            _actionTypes[actionType].first++;
        } else {
            _actionTypes[actionType].first = 1;

            emit actionTypeAdded(actionType);
        }

        if (getActionTypes() != cachedActionTypes) {
            emit actionTypesChanged(getActionTypes());
            emit actionTypesHumanFriendlyChanged(getActionTypesHumanFriendly());
        }
    }

    /**
     * Remove \p actionType string
     * @param actionType Action type string to remove
     */
    void removeActionType(const QString& actionType) {
        const auto cachedActionTypes = getActionTypes();

        if (!_actionTypes.contains(actionType))
            return;

        _actionTypes[actionType].first--;

        if (_actionTypes[actionType].first <= 0) {
            _actionTypes.remove(actionType);

            emit actionTypeRemoved(actionType);
        }

        if (getActionTypes() != cachedActionTypes) {
            emit actionTypesChanged(getActionTypes());
            emit actionTypesHumanFriendlyChanged(getActionTypesHumanFriendly());
        }
    }

public: // Action types

    /**
     * Get set of action types
     * @return List of action types
     */
    const QStringList getActionTypes() const {
        auto actionTypes = _actionTypes.keys();

        actionTypes.removeDuplicates();

        return actionTypes;
    }

    /**
     * Get set of human-friendly action types (without prefixes)
     * @return List of human-friendly action types
     */
    const QStringList getActionTypesHumanFriendly() const {
        QStringList actionTypesHumanFriendly;

        for (const auto& actionType : _actionTypes)
            actionTypesHumanFriendly << actionType.second;

        return actionTypesHumanFriendly;
    }

protected:

    /**
     * Make widget \p action public
     * @param action Pointer to action
     */
    virtual void makeActionPublic(gui::WidgetAction* action) final;

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
     * Signals that \p publicAction is added to the manager
     * @param action Pointer to public action that is added to the manager
     */
    void publicActionAdded(gui::WidgetAction* publicAction);

    /**
     * Signals that \p publicAction is about to be removed from the manager
     * @param action Pointer to public action that is about to be removed from the manager
     */
    void publicActionAboutToBeRemoved(gui::WidgetAction* publicAction);

    /**
     * Signals that public action with \p publicActionId was removed from the manager
     * @param publicActionId Globally unique identifier of the public action that was removed from the manager
     */
    void publicActionRemoved(const QString& publicActionId);

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

    /**
     * Signals that the human-friendly \p actionTypes changed
     * @param actionTypesHumanFriendly Human-friendly action types
     */
    void actionTypesHumanFriendlyChanged(const QStringList& actionTypesHumanFriendly);

protected:
    gui::WidgetActions          _actions;               /** List of actions that are instantiated in the plugin system */
    gui::WidgetActions          _publicActions;         /** List of public actions that are instantiated in the plugin system */
    QMap<QString, ActionType>   _actionTypes;           /** Maps action type to counter and human-friendly action type string */

    friend class gui::WidgetAction;
};

}

#endif // ABSTRACT_ACTION_MANAGER_H