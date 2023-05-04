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

    auto actionItemParent = actionItem->parent();

    if (actionItemParent)
        actionItemParent->removeRow(actionItem->row());
    else
        removeRow(actionItem->row());
}

void PublicActionsModel::addPublicAction(WidgetAction* action)
{
    appendRow(Row(action));

    const auto actionIndex = getActionIndex(action);

    if (!actionIndex.isValid())
        return;

    auto actionItem = itemFromIndex(actionIndex);

    Q_ASSERT(actionItem != nullptr);

    if (!actionItem)
        return;

    for (auto connectedAction : action->getConnectedActions())
        actionItem->appendColumn(Row(connectedAction));

    connect(action, &WidgetAction::actionConnected, this, [this](WidgetAction* action) -> void {
        addAction(action);
    });

    connect(action, &WidgetAction::actionDisconnected, this, [this](WidgetAction* action) -> void {
        removeAction(action);
    });
}

void PublicActionsModel::removePublicAction(WidgetAction* publicAction)
{
    disconnect(publicAction, &WidgetAction::actionConnected, this, nullptr);
    disconnect(publicAction, &WidgetAction::actionDisconnected, this, nullptr);
}

}