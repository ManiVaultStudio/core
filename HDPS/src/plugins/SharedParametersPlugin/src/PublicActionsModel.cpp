#include "PublicActionsModel.h"

#ifdef _DEBUG
    #define PUBLIC_ACTIONS_MODEL_VERBOSE
#endif

using namespace hdps::gui;

namespace hdps
{

PublicActionsModel::PublicActionsModel(QObject* parent) :
    AbstractActionsModel(parent)
{
    initialize();
}

void PublicActionsModel::initialize()
{
    setRowCount(0);

    for (auto action : actions().getActions())
        addAction(action);
}

void PublicActionsModel::actionAddedToManager(WidgetAction* action)
{
    addAction(action);
}

void PublicActionsModel::actionAboutToBeRemovedFromManager(WidgetAction* action)
{
    removeAction(action);
}

void PublicActionsModel::publicActionAddedToManager(WidgetAction* publicAction)
{
    addPublicAction(publicAction);
}

void PublicActionsModel::publicActionAboutToBeRemovedFromManager(WidgetAction* publicAction)
{
    removePublicAction(publicAction);
}

void PublicActionsModel::addAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    if (action->isPrivate() && action->isConnected()) {
        auto publicAction = action->getPublicAction();

        if (!publicAction)
            return;

//#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
//        qDebug() << __FUNCTION__ << publicAction->getId() << action->getId();
//#endif

        auto publicActionItem = getActionItem(publicAction);

        if (!publicActionItem)
            return;

        auto row = Row(action);

        publicActionItem->appendRow(row);

        if (action->isPrivate()) {
            QFont font;

            font.setItalic(true);

            for (auto item : row)
                item->setFont(font);
        }
    }
}

void PublicActionsModel::removeAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    auto actionItem = getActionItem(action);

    if (!actionItem)
        return;

    //if (action->isPublic())
    //    removeRow(actionItem->row());

    if (action->isPrivate()) {
        auto publicActionItem = getActionItem(action->getPublicAction());

        if (publicActionItem)
            publicActionItem->removeRow(actionItem->row());
    }
}

void PublicActionsModel::addPublicAction(WidgetAction* publicAction)
{
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << publicAction->text();
#endif

    auto parentActionItem = getActionItem(publicAction->getParentAction());

    if (parentActionItem)
        parentActionItem->appendRow(Row(publicAction));
    else
        appendRow(Row(publicAction));

    auto actionItem = getActionItem(publicAction);

    //Q_ASSERT(actionItem != nullptr);

    //if (actionItem) {
    //    for (auto connectedAction : action->getConnectedActions())
    //        actionItem->appendRow(Row(connectedAction));
    //}

    connect(publicAction, &WidgetAction::actionConnected, this, [this](WidgetAction* action) -> void {
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
        qDebug() << action->getLocation() << " connected to " << action->getPublicAction()->getLocation();
#endif

        addAction(action);
    });

    connect(publicAction, &WidgetAction::actionDisconnected, this, [this](WidgetAction* action) -> void {
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
        qDebug() << action->getLocation() << " disconnected from " << action->getPublicAction()->getLocation();
#endif

        removeAction(action);
    });
}

void PublicActionsModel::removePublicAction(WidgetAction* publicAction)
{
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << publicAction->text();
#endif

    Q_ASSERT(publicAction != nullptr);

    if (publicAction == nullptr)
        return;

    disconnect(publicAction, &WidgetAction::actionConnected, this, nullptr);
    disconnect(publicAction, &WidgetAction::actionDisconnected, this, nullptr);

    auto publicActionItem = getActionItem(publicAction);

    Q_ASSERT(publicActionItem != nullptr);

    if (!publicActionItem)
        return;

    auto parentItem = publicActionItem->parent();

    if (parentItem)
        parentItem->removeRow(publicActionItem->row());
    else
        removeRow(publicActionItem->row());
}

}