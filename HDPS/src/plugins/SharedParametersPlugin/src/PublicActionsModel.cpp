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

    if (!action->isConnected())
        return;

    auto publicAction = action->getPublicAction();

    if (!publicAction)
        return;

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

void PublicActionsModel::removeAction(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return;

    auto actionItem = getActionItem(action);

    if (!actionItem)
        return;

    if (action->isPublic())
        removeRow(actionItem->row());

    if (action->isPrivate()) {
        auto publicActionItem = getActionItem(action->getPublicAction());

        if (publicActionItem)
            publicActionItem->removeRow(actionItem->row());
    }
}

void PublicActionsModel::addPublicAction(WidgetAction* action)
{
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    if (action->getParentAction()) {
        auto parentActionItem = getActionItem(action->getParentAction());

        if (parentActionItem)
            parentActionItem->appendRow(Row(action));
        else
            appendRow(Row(action));
    }
    else {
        appendRow(Row(action));
    }
    
    auto actionItem = getActionItem(action);

    Q_ASSERT(actionItem != nullptr);

    if (actionItem) {
        for (auto connectedAction : action->getConnectedActions())
            actionItem->appendRow(Row(connectedAction));
    }

    connect(action, &WidgetAction::actionConnected, this, [this](WidgetAction* action) -> void {
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
        qDebug() << action->text() << " connected to " << action->getPublicAction()->text();
#endif

        addAction(action);
    });

    connect(action, &WidgetAction::actionDisconnected, this, [this](WidgetAction* action) -> void {
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
        qDebug() << action->text() << " disconnected from " << action->getPublicAction()->text();
#endif

        removeAction(action);
    });
}

void PublicActionsModel::removePublicAction(WidgetAction* publicAction)
{
    disconnect(publicAction, &WidgetAction::actionConnected, this, nullptr);
    disconnect(publicAction, &WidgetAction::actionDisconnected, this, nullptr);
}

}