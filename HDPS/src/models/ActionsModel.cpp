#include "ActionsModel.h"
#include "AbstractActionsManager.h"

#include <actions/WidgetAction.h>

#include <Application.h>

#include <QTimer>

using namespace hdps::gui;

#ifdef _DEBUG
    #define ACTIONS_MODEL_VERBOSE
#endif

namespace hdps
{

QMap<ActionsModel::Column, QPair<QString, QString>> ActionsModel::columnInfo = QMap<ActionsModel::Column, QPair<QString, QString>>({
    { ActionsModel::Column::Name, { "Name", "Action name" }},
    { ActionsModel::Column::ID, { "ID", "Globally unique identifier of the action" }},
    { ActionsModel::Column::Type, { "Type", "Type of action" }},
    { ActionsModel::Column::Scope, { "Scope", "Scope of the action (whether the action is public or private)" }},
    { ActionsModel::Column::Connected, { "Connected", "Whether the action is connected or not" }}
});

ActionsModel::Item::Item(ActionsModel* actionsModel, gui::WidgetAction* widgetAction, const ActionsModel::Column& column) :
    QStandardItem(),
    _actionsModel(actionsModel),
    _widgetAction(widgetAction),
    _column(column)
{
    Q_ASSERT(_widgetAction != nullptr);

    setData(QVariant::fromValue(widgetAction));

    //const auto updateReadOnly = [this]() -> void {
    //    setForeground(_widgetAction->isEnabled() ? Qt::black : Qt::gray);
    //};

    //connect(_widgetAction, &WidgetAction::changed, this, updateReadOnly);
    //connect(_widgetAction, &WidgetAction::scopeChanged, this, updateReadOnly);

    switch (_column) {
        case Column::Name:
        {
            //setIcon(Application::getIconFont("FontAwesome").getIcon("link"));

            updateNameColumn();

            connect(_widgetAction, &WidgetAction::changed, this, &Item::updateNameColumn);

            connect(_widgetAction, &WidgetAction::scopeChanged, this, [this]() -> void {
                if (_widgetAction->isPublic())
                    setData(_widgetAction->text(), Qt::EditRole);
                
                //setCheckState(_widgetAction->isEnabled() ? Qt::Checked : Qt::Unchecked);
                //setCheckable(_widgetAction->isPublic());
                setEditable(_widgetAction->isPublic());
            });

            connect(_widgetAction, &WidgetAction::actionConnected, this, [this, actionsModel](WidgetAction* action) -> void {
                const QList<QStandardItem*> row {
                    new Item(actionsModel, action, Column::Name),
                    new Item(actionsModel, action, Column::ID),
                    new Item(actionsModel, action, Column::Type),
                    new Item(actionsModel, action, Column::Scope),
                    new Item(actionsModel, action, Column::Connected)
                };

                for (auto item : row)
                    item->setEditable(false);

                row[static_cast<int>(Column::Name)]->setToolTip(_widgetAction->getPath());

                appendRow(row);
            });

            connect(_widgetAction, &WidgetAction::actionDisconnected, this, [this, actionsModel](WidgetAction* action) -> void {
                for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++) {
                    auto item = static_cast<Item*>(child(rowIndex));

                    if (action == item->getAction())
                        removeRow(rowIndex);
                }
            });

            break;
        }

        case Column::ID:
        {
            updateIdColumn();

            break;
        }

        case Column::Type:
        {
            updateTypeColumn();

            QTimer::singleShot(10, this, &Item::updateTypeColumn);

            break;
        }

        case Column::Scope:
        {
            updateScopeColumn();

            connect(_widgetAction, &WidgetAction::scopeChanged, this, &Item::updateScopeColumn);

            break;
        }

        case Column::Connected:
        {
            updateConnectedColumn();

            connect(_widgetAction, &WidgetAction::isConnectedChanged, this, &Item::updateConnectedColumn);

            break;
        }

        default:
            break;
    }
}

void ActionsModel::Item::updateNameColumn()
{
    if (_widgetAction.isNull())
        return;

    if (_widgetAction->isPrivate())
        setText(_widgetAction->getPath());

    //if (_widgetAction->isPrivate())
    //    setText(_widgetAction->text());

    setEditable(_widgetAction->isPublic());
}

void ActionsModel::Item::updateIdColumn()
{
    if (_widgetAction.isNull())
        return;

    setText(_widgetAction->getId());
}

void ActionsModel::Item::updateTypeColumn()
{
    if (_widgetAction.isNull())
        return;

    const auto typeString               = _widgetAction->getTypeString();
    const auto humanFriendlyTypeString  = _widgetAction->getTypeString(true);

    setData(typeString, Qt::EditRole);
    setData(humanFriendlyTypeString, Qt::DisplayRole);

    _actionsModel->addActionType(humanFriendlyTypeString);
}

void ActionsModel::Item::updateScopeColumn()
{
    if (_widgetAction.isNull())
        return;

    setText(WidgetAction::scopeNames[_widgetAction->getScope()]);
    setData(QVariant::fromValue(static_cast<int>(_widgetAction->getScope())), Qt::UserRole + 1);
}

void ActionsModel::Item::updateConnectedColumn()
{
    if (_widgetAction.isNull())
        return;

    const auto isConnected = _widgetAction->isConnected();

    setText(isConnected ? "Yes" : "No");
    //setData(QVariant::fromValue(isConnected), Qt::EditRole);
}

WidgetAction* ActionsModel::Item::getAction()
{
    return _widgetAction;
}

ActionsModel::Item::~Item()
{
    if (_column == Column::Type)
        _actionsModel->removeActionType(this->data(Qt::DisplayRole).toString());
}

QVariant ActionsModel::Item::data(int role /*= Qt::UserRole + 1*/) const
{

}

ActionsModel::ActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent),
    _actions(),
    _actionTypes(),
    add(0),
    remove(0)
{
    setColumnCount(static_cast<int>(Column::Count));

    const auto setHeader = [this](Column column) -> void {
        auto headerItem = new QStandardItem(columnInfo[column].first);

        headerItem->setToolTip(columnInfo[column].second);

        setHorizontalHeaderItem(static_cast<int>(column), headerItem);
    };

    for (auto column : columnInfo.keys())
        setHeader(column);

    connect(this, &QStandardItemModel::itemChanged, this, [this](QStandardItem* standardItem) -> void {
        auto actionItem = static_cast<Item*>(standardItem);

        if (actionItem->column() == static_cast<int>(Column::Name)) {
            if (actionItem->getAction()->isPrivate())
                return;

            //if (standardItem->isCheckable())
            //    actionItem->getAction()->setEnabled(standardItem->checkState() == Qt::Checked);

            actionItem->getAction()->setText(standardItem->data(Qt::EditRole).toString());
        }
    });
}

void ActionsModel::addAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->getId();
#endif

    _actions << action;

    if (action->getId().startsWith("6d6e"))
        qDebug() << "FOUND_6E6D";

    const QList<QStandardItem*> row {
        new Item(this, action, Column::Name),
        new Item(this, action, Column::ID),
        new Item(this, action, Column::Type),
        new Item(this, action, Column::Scope),
        new Item(this, action, Column::Connected)
    };

    for (auto item : row)
        item->setEditable(false);
    
    const auto parentAction = action->getParentWidgetAction();

    if (parentAction) {
        const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::DisplayRole, parentAction->getId(), 1, Qt::MatchFlag::MatchRecursive);

        if (matches.count() != 1)
            appendRow(row);
        else {
            const auto parentItem = itemFromIndex(matches.first().siblingAtColumn(static_cast<int>(Column::Name)));

            if (parentItem != nullptr)
                parentItem->appendRow(row);
        }
    }
    else {
        appendRow(row);
    }

    add++;
}

void ActionsModel::removeAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->getId();
#endif

    if (_actions.contains(action))
        _actions.removeOne(action);

    removed << action->getId();

    auto text = action->text();
    auto rc = rowCount();

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return;

    QList<QPersistentModelIndex> persistentMatches;

    for (const auto& match : matches)
        persistentMatches << QPersistentModelIndex(match);

    for (const auto& persistentMatch : persistentMatches)
        qDebug() << removeRow(persistentMatch.row(), persistentMatch.parent());

    const auto matches2 = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, "*", -1, Qt::MatchFlag::MatchRecursive | Qt::MatchFlag::MatchWildcard);

    //QStringList ids2;

    //for (const auto& match : matches2)
    //    ids2 << match.data().toString();

    //qDebug() << ids2.count();

    //ids2.removeDuplicates();

    //qDebug() << ids2.count();

    remove++;

    qDebug() << "*Number of actions:" << matches.count() << matches2.count() << _actions.count() << removed.count() << add << remove;
}

WidgetActions ActionsModel::getActions() const
{
    WidgetActions actions;

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, "*", -1, Qt::MatchFlag::MatchRecursive | Qt::MatchFlag::MatchWildcard);

    qDebug() << "*Number of actions:" << matches.count() << _actions.count();

    for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
        actions << static_cast<Item*>(item(rowIndex, 0))->getAction();

    return actions;
}

void ActionsModel::addActionType(const QString& actionType)
{
    const auto cachedActionTypes = getActionTypes();

    if (_actionTypes.contains(actionType))
        _actionTypes[actionType]++;
    else
        _actionTypes[actionType] = 1;

    if (getActionTypes() != cachedActionTypes)
        emit actionTypesChanged(getActionTypes());
}

void ActionsModel::removeActionType(const QString& actionType)
{
    const auto cachedActionTypes = getActionTypes();

    if (!_actionTypes.contains(actionType))
        return;

    _actionTypes[actionType]--;

    if (_actionTypes[actionType] <= 0)
        _actionTypes.remove(actionType);

    if (getActionTypes() != cachedActionTypes)
        emit actionTypesChanged(getActionTypes());
}

const QStringList ActionsModel::getActionTypes() const
{
    auto actionTypes = _actionTypes.keys();

    actionTypes.removeDuplicates();

    return actionTypes;
}

WidgetAction* ActionsModel::getAction(std::int32_t rowIndex)
{
    return static_cast<Item*>(item(rowIndex, 0))->getAction();
}

WidgetAction* ActionsModel::getAction(const QModelIndex& index)
{
    return static_cast<Item*>(itemFromIndex(index))->getAction();
}

}
