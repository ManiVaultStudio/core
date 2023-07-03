#include "AbstractActionsManager.h"


namespace hdps
{
    void AbstractActionsManager::connectPrivateActionToPublicAction(gui::WidgetAction* privateAction, gui::WidgetAction* publicAction, bool recursive) {
        Q_ASSERT(privateAction != nullptr);
        Q_ASSERT(publicAction != nullptr);

        if (privateAction == nullptr || publicAction == nullptr)
            return;

        try
        {
            if (privateAction->isConnectionPermissionFlagSet(gui::WidgetAction::ConnectionPermissionFlag::ForceNone))
                return;

            if (privateAction->isConnected() && (privateAction->getPublicAction() == publicAction))
                throw std::runtime_error(QString("%1 is already connected to %2").arg(privateAction->getLocation(), publicAction->getLocation()).toLatin1());

            privateAction->connectToPublicAction(publicAction, recursive);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to connect %1 to %2:").arg(privateAction->text(), publicAction->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to connect %1 to %2:").arg(privateAction->text(), publicAction->text()));
        }
    }

    void AbstractActionsManager::connectPrivateActions(gui::WidgetAction* privateSourceAction, gui::WidgetAction* privateTargetAction, const QString& publicActionName) {
        Q_ASSERT(privateSourceAction != nullptr);
        Q_ASSERT(privateTargetAction != nullptr);

        if (privateSourceAction == nullptr || privateTargetAction == nullptr)
            return;

        try
        {
            privateTargetAction->publish(publicActionName);
            privateSourceAction->connectToPublicAction(privateTargetAction->getPublicAction(), true);
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to connect %1 to %2:").arg(privateSourceAction->text(), privateTargetAction->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to connect %1 to %2:").arg(privateSourceAction->text(), privateTargetAction->text()));
        }
    }

    void AbstractActionsManager::disconnectPrivateActionFromPublicAction(gui::WidgetAction* privateAction, bool recursive) {
        Q_ASSERT(privateAction != nullptr);

        if (privateAction == nullptr)
            return;
        
        try
        {
            if (privateAction->isConnected())
                privateAction->disconnectFromPublicAction(recursive);

            privateAction->_publicAction = nullptr;
        }
        catch (std::exception& e)
        {
            util::exceptionMessageBox(QString("Unable to disconnect %1 to %2:").arg(privateAction->text(), privateAction->getPublicAction()->text()), e);
        }
        catch (...)
        {
            util::exceptionMessageBox(QString("Unable to disconnect %1 to %2:").arg(privateAction->text(), privateAction->getPublicAction()->text()));
        }
    }

    void AbstractActionsManager::makeActionPublic(gui::WidgetAction* action) {
        Q_ASSERT(action != nullptr);

        if (action == nullptr)
            return;

        action->makePublic();
    }

}
