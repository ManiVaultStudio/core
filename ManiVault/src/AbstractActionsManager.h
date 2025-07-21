// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "AbstractManager.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

#include <QObject>

#ifdef _DEBUG
    #define ABSTRACT_ACTIONS_MANAGER_VERBOSE
#endif

namespace mv
{

class ActionsListModel;

/**
 * Abstract actions manager
 *
 * Base abstract actions manager class for managing widget actions.
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AbstractActionsManager : public AbstractManager
{
    Q_OBJECT

private:

    /** Number of instances for action type and human-friendly action type string */
    using ActionType = QPair<std::int32_t, QString>;

public:

    /**
     * Construct manager with pointer to \p parent object
     * @param parent Pointer to parent object
     */
    AbstractActionsManager(QObject* parent) :
        AbstractManager(parent, "Actions")
    {
    }

    /**
     * Get all actions in the manager
     * @return List of all actions in the manager
     */
    const gui::WidgetActions& getActions() const {
        return _actions;
    }

    /**
     * Get all public actions in the manager
     * @return List of all public actions in the manager
     */
    const gui::WidgetActions& getPublicActions() const {
        return _publicActions;
    }

    /**
     * Get action with \p id
     * @return Pointer to widget action (may return nullptr)
     */
    gui::WidgetAction* getAction(const QString& id) {
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
        try
        {
            Q_ASSERT(action != nullptr);

            if (action == nullptr)
                throw std::runtime_error("Action may not be a null pointer");

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
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to add action to actions manager", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to add action to actions manager");
        }
    }

    /**
     * Remove \p action of action type from the manager
     * @param action Pointer to action
     */
    template<typename ActionType>
    void removeAction(ActionType* action) {

        if (isCoreDestroyed())
            return;

        try
        {
            Q_ASSERT(action != nullptr);

            if (action == nullptr)
                throw std::runtime_error("Action may not be a null pointer");

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
            util::exceptionMessageBox("Unable to remove action from actions manager", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to remove action from actions manager");
        }
    }

private: // Public actions

    /**
     * Add \p publicAction to the manager
     * @param publicAction Pointer to public action
     */
    void addPublicAction(gui::WidgetAction* publicAction) {
        try
        {
            Q_ASSERT(publicAction != nullptr);

            if (publicAction == nullptr)
                throw std::runtime_error("Public action may not be a null pointer");

            _publicActions << publicAction;

            emit publicActionAdded(publicAction);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to add public action:", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to add public action:");
        }
    }

    /**
     * Remove \p publicAction from the manager
     * @param publicAction Pointer to public action
     */
    void removePublicAction(gui::WidgetAction* publicAction) {
        try
        {
            Q_ASSERT(publicAction != nullptr);

            if (publicAction == nullptr)
                throw std::runtime_error("Public action may not be a null pointer");

            if (!_publicActions.contains(publicAction))
                throw std::runtime_error(QString("%1 not found").arg(publicAction->text()).toStdString());

            emit publicActionAboutToBeRemoved(publicAction);
            {
                for (auto connectedAction : publicAction->getConnectedActions())
                    disconnectPrivateActionFromPublicAction(connectedAction, true);

                _publicActions.removeOne(publicAction);
            }
            emit publicActionRemoved(publicAction->getId());
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to remove public action:", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to remove public action:");
        }
    }

public: // Linking

    /**
     * Publish \p privateAction so that other private actions can connect to it
     * @param privateAction Pointer to private action to publish
     * @param name Name of the published widget action (if empty, a name choosing dialog will popup)
     * @param recursive Whether to also publish the child actions recursively
     * @param allowDuplicateName Boolean determining whether publishing will take place when a public with the same name already exists in the public actions database
     * @return Boolean determining whether the action is successfully published or not
     */
    virtual bool publishPrivateAction(gui::WidgetAction* privateAction, const QString& name = "", bool recursive = true, bool allowDuplicateName = false) = 0;

    /**
     * Connect \p privateAction to \p publicAction
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     * @param recursive Whether to also connect descendant child actions
     */
    void connectPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction, bool recursive) {
        
        try
        {
            Q_ASSERT(privateAction != nullptr);
            Q_ASSERT(publicAction != nullptr);

            if (privateAction == nullptr)
                throw std::runtime_error("Private action may not be a null pointer");

            if (publicAction == nullptr)
                throw std::runtime_error("Public action may not be a null pointer");

            if (!privateAction->mayConnectToPublicAction(publicAction))
                throw std::runtime_error(QString("%1 may not be connected to %2").arg(privateAction->getLocation(), publicAction->getLocation()).toStdString());

            if (privateAction->isConnectionPermissionFlagSet(gui::WidgetAction::ConnectionPermissionFlag::ForceNone))
                return;

            if (privateAction->isConnected() && (privateAction->getPublicAction() == publicAction))
                throw std::runtime_error(QString("%1 is already connected to %2").arg(privateAction->getLocation(), publicAction->getLocation()).toStdString());

            privateAction->connectToPublicAction(publicAction, recursive);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to connect private action to public action", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to connect private action to public action");
        }
    }

    /**
     * Connect \p privateSourceAction to \p privateTargetAction
     * @param privateSourceAction Pointer to private source action
     * @param privateTargetAction Pointer to private target action (private source action will be connected to published private target action)
     * @param publicActionName Name of the public action (ask for name if empty)
     */
    void connectPrivateActions(gui::WidgetAction* privateSourceAction, gui::WidgetAction* privateTargetAction, const QString& publicActionName = "") {

        try
        {
            Q_ASSERT(privateSourceAction != nullptr);
            Q_ASSERT(privateTargetAction != nullptr);

            if (privateSourceAction == nullptr)
                throw std::runtime_error("Private source action may not be a null pointer");

            if (privateTargetAction == nullptr)
                throw std::runtime_error("Private target action may not be a null pointer");

            if (privateTargetAction->publish(publicActionName))
                privateSourceAction->connectToPublicAction(privateTargetAction->getPublicAction(), true);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to connect private actions", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to connect private actions");
        }
    }

    /**
     * Disconnect \p privateAction from public action
     * @param privateAction Pointer to private action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectPrivateActionFromPublicAction(gui::WidgetAction* privateAction, bool recursive) {

        try
        {
            Q_ASSERT(privateAction != nullptr);

            if (privateAction == nullptr)
                throw std::runtime_error("Private action may not be a null pointer");
        
            if (privateAction->isConnected())
                privateAction->disconnectFromPublicAction(recursive);

            privateAction->_publicAction = nullptr;
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to disconnect private action from public action:", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to disconnect private action from public action:");
        }
    }

protected:

    /**
     * Add \p privateAction to the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    void addPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) {

        try
        {
            Q_ASSERT(privateAction != nullptr);
            Q_ASSERT(publicAction != nullptr);

            if (privateAction == nullptr)
                throw std::runtime_error("Private action may not be a null pointer");

            if (publicAction == nullptr)
                throw std::runtime_error("Public action may not be a null pointer");

            publicAction->getConnectedActions() << privateAction;

            emit publicAction->actionConnected(privateAction);

            connect(privateAction, &gui::WidgetAction::destroyed, this, [this, privateAction, publicAction]() -> void {
                removePrivateActionFromPublicAction(privateAction, publicAction);
            });
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to add private action to public action:", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to add private action to public action:");
        }
    }

    /**
     * Remove \p privateAction from the list of connected actions in the \p publicAction and notify listeners
     * @param privateAction Pointer to private action
     * @param publicAction Pointer to public action
     */
    void removePrivateActionFromPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction) {
        
        try
        {
            Q_ASSERT(privateAction != nullptr);
            Q_ASSERT(publicAction != nullptr);

            if (privateAction == nullptr)
                throw std::runtime_error("Private action may not be a null pointer");

            if (publicAction == nullptr)
                throw std::runtime_error("Public action may not be a null pointer");

            publicAction->getConnectedActions().removeOne(privateAction);

            emit publicAction->actionDisconnected(privateAction);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox("Unable to remove private action from public action:", e);
        }
        catch (...)
        {
            util::exceptionMessageBox("Unable to remove private action from public action:");
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
        if (isCoreDestroyed())
            return;

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
    QStringList getActionTypes() const {
        auto actionTypes = _actionTypes.keys();

        actionTypes.removeDuplicates();

        return actionTypes;
    }

    /**
     * Get set of human-friendly action types (without prefixes)
     * @return List of human-friendly action types
     */
    QStringList getActionTypesHumanFriendly() const {
        QStringList actionTypesHumanFriendly;

        for (const auto& actionType : _actionTypes)
            actionTypesHumanFriendly << actionType.second;

        return actionTypesHumanFriendly;
    }

public: // Models

    virtual ActionsListModel& getActionsListModel() = 0;

protected:

    /**
     * Make widget \p action public
     * @param action Pointer to action
     */
    void makeActionPublic(gui::WidgetAction* action) {
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
     * Signals that \p publicAction is added to the manager
     * @param publicAction Pointer to public action that is added to the manager
     */
    void publicActionAdded(gui::WidgetAction* publicAction);

    /**
     * Signals that \p publicAction is about to be removed from the manager
     * @param publicAction Pointer to public action that is about to be removed from the manager
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
