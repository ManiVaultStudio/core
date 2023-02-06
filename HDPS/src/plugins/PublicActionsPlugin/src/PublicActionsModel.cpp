#include "PublicActionsModel.h"

#include <CoreInterface.h>

#include <actions/WidgetAction.h>

#include <QDebug>

#ifdef _DEBUG
    #define PUBLIC_ACTIONS_MODEL_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

QMap<PublicActionsModel::Column, QPair<QString, QString>> PublicActionsModel::columnInfo = QMap<PublicActionsModel::Column, QPair<QString, QString>>({
    { PublicActionsModel::Column::Name, { "Name", "Name of the widget action" }},
    { PublicActionsModel::Column::ID, { "ID", "Widget action identifier" }}
});

PublicActionsModel::PublicActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    const auto setHeader = [this](Column column) -> void {
        auto headerItem = new QStandardItem(columnInfo[column].first);

        headerItem->setToolTip(columnInfo[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), headerItem);
    };

    setHeader(Column::Name);
    setHeader(Column::ID);

    connect(&actions(), &AbstractActionsManager::publicActionAdded, this, &PublicActionsModel::addPublicAction);
    connect(&actions(), &AbstractActionsManager::publicActionAboutToBeRemoved, this, &PublicActionsModel::removePublicAction);

    for (auto action : actions().getActions())
        addPublicAction(action);
}

void PublicActionsModel::addPublicAction(WidgetAction* action)
{
    if (!action->isPublic())
        return;

#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const QList<QStandardItem*> publicActionItems{
        new QStandardItem(action->text()),
        new QStandardItem(action->getId()),
    };

    /*
    for (auto connectedAction : action->getConnectedActions()) {
        const QList<QStandardItem*> connectedActionItems{
            new QStandardItem(connectedAction->getSettingsPath()),
            new QStandardItem(connectedAction->getId()),
        };

        //connect(connectedAction, &Widget)
        publicActionItems.first()->appendRow(connectedActionItems);
    }
    */

    appendRow(publicActionItems);
}

void PublicActionsModel::removePublicAction(hdps::gui::WidgetAction* action)
{
#ifdef PUBLIC_ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::DisplayRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return;

    for (const auto& match : matches)
        removeRow(match.row(), match.parent());
}
