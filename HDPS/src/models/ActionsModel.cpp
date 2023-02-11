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

ActionsModel::Item::Item(ActionsModel* actionsModel, gui::WidgetAction* widgetAction, const ActionsModel::Column& column) :
    QStandardItem(),
    _actionsModel(actionsModel),
    _widgetAction(widgetAction),
    _column(column)
{
    Q_ASSERT(_widgetAction != nullptr);

    setData(QVariant::fromValue(widgetAction));

    const auto updateReadOnly = [this]() -> void {
        setForeground(_widgetAction->isEnabled() ? Qt::black : Qt::gray);
    };

    connect(_widgetAction, &WidgetAction::changed, this, updateReadOnly);
    connect(_widgetAction, &WidgetAction::scopeChanged, this, updateReadOnly);

    switch (_column) {
        case Column::Name:
        {
            //setIcon(Application::getIconFont("FontAwesome").getIcon("link"));

            updateName();

            connect(_widgetAction, &WidgetAction::changed, this, &Item::updateName);

            connect(_widgetAction, &WidgetAction::scopeChanged, this, [this]() -> void {
                if (_widgetAction->isPublic())
                    setData(_widgetAction->text(), Qt::EditRole);
                
                setCheckState(_widgetAction->isEnabled() ? Qt::Checked : Qt::Unchecked);
                setCheckable(_widgetAction->isPublic());
                setEditable(_widgetAction->isPublic());
            });

            connect(_widgetAction, &WidgetAction::actionConnected, this, [this, actionsModel](WidgetAction* action) -> void {
                const QList<QStandardItem*> row {
                    new Item(actionsModel, action, Column::Name),
                    new Item(actionsModel, action, Column::ID),
                    new Item(actionsModel, action, Column::Type),
                    new Item(actionsModel, action, Column::Scope)
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
            updateId();

            break;
        }

        case Column::Type:
        {
            updateType();

            QTimer::singleShot(10, this, &Item::updateType);

            break;
        }

        case Column::Scope:
        {
            updateScope();

            connect(_widgetAction, &WidgetAction::scopeChanged, this, &Item::updateScope);

            break;
        }

        default:
            break;
    }
}

void ActionsModel::Item::updateName()
{
    if (_widgetAction.isNull())
        return;

    //if (_widgetAction->isPrivate())
    //    setText(_widgetAction->getPath());

    if (_widgetAction->isPrivate())
        setText(_widgetAction->text());

    setEditable(_widgetAction->isPublic());
}

void ActionsModel::Item::updateId()
{
    if (_widgetAction.isNull())
        return;

    setText(_widgetAction->getId());
}

void ActionsModel::Item::updateType()
{
    if (_widgetAction.isNull())
        return;

    const auto typeString               = _widgetAction->getTypeString();
    const auto humanFriendlyTypeString  = _widgetAction->getTypeString(true);

    setData(typeString, Qt::EditRole);
    setData(humanFriendlyTypeString, Qt::DisplayRole);

    _actionsModel->addActionType(humanFriendlyTypeString);
}

void ActionsModel::Item::updateScope()
{
    if (_widgetAction.isNull())
        return;

    setText(WidgetAction::scopeNames[_widgetAction->getScope()]);
    setData(QVariant::fromValue(static_cast<int>(_widgetAction->getScope())), Qt::UserRole + 1);
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

ActionsModel::ActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    setHeaderData(static_cast<int>(Column::Name), Qt::Horizontal, "Name");
    setHeaderData(static_cast<int>(Column::ID), Qt::Horizontal, "ID");
    setHeaderData(static_cast<int>(Column::Type), Qt::Horizontal, "Type");
    setHeaderData(static_cast<int>(Column::Scope), Qt::Horizontal, "Scope");

    setHeaderData(static_cast<int>(Column::Name), Qt::Horizontal, "Name of the parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::ID), Qt::Horizontal, "Globally unique identifier of the parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::Type), Qt::Horizontal, "Type of parameter", Qt::ToolTipRole);
    setHeaderData(static_cast<int>(Column::Scope), Qt::Horizontal, "Whether the parameter is public or private", Qt::ToolTipRole);

    connect(this, &QStandardItemModel::itemChanged, this, [this](QStandardItem* standardItem) -> void {
        auto actionItem = static_cast<Item*>(standardItem);

        if (actionItem->column() == static_cast<int>(Column::Name)) {
            if (actionItem->getAction()->isPrivate())
                return;

            if (standardItem->isCheckable())
                actionItem->getAction()->setEnabled(standardItem->checkState() == Qt::Checked);

            actionItem->getAction()->setText(standardItem->data(Qt::EditRole).toString());
        }
    });
}

void ActionsModel::addAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const QList<QStandardItem*> row {
        new Item(this, action, Column::Name),
        new Item(this, action, Column::ID),
        new Item(this, action, Column::Type),
        new Item(this, action, Column::Scope)
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
}

void ActionsModel::removeAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::DisplayRole, action->getId(), -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return;

    for (const auto& match : matches)
        removeRow(match.row(), match.parent());
}

const WidgetActions& ActionsModel::getActions() const
{
    WidgetActions actions;

    //for (int rowIndex = 0; rowIndex < rowCount(); rowIndex++)
    //    actions << static_cast<Item*>(item(rowIndex, 0))->getAction();

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
