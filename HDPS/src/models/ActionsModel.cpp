#include "ActionsModel.h"
#include "AbstractActionsManager.h"

#include <actions/WidgetAction.h>

#include <Application.h>

#include <QTimer>

using namespace hdps::gui;

#ifdef _DEBUG
    //#define ACTIONS_MODEL_VERBOSE
#endif

namespace hdps
{

ActionsModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant ActionsModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return _columHeaderInfo._display;

        case Qt::EditRole:
            return _columHeaderInfo._edit;

        case Qt::ToolTipRole:
            return _columHeaderInfo._tooltip;

        default:
            break;
    }

    return QVariant();
}

ActionsModel::Item::Item(gui::WidgetAction* action, bool editable /*= false*/) :
    _action(action)
{
    Q_ASSERT(_action != nullptr);

    setEditable(editable);
}

QPointer<hdps::gui::WidgetAction> ActionsModel::Item::getAction() const
{
    return _action;
}

QVariant ActionsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    return getAction()->getPath();
}

QVariant ActionsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    return getAction()->getId();
}

QVariant ActionsModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->getTypeString();

        case Qt::DisplayRole:
            return getAction()->getTypeString(true);
    }

    return QVariant();
}

QVariant ActionsModel::ScopeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getAction()->getScope());

        case Qt::DisplayRole:
            return WidgetAction::scopeNames[getAction()->getScope()];
    }

    return QVariant();
}

QVariant ActionsModel::IsConnectedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->isConnected();

        case Qt::DisplayRole:
            return getAction()->isConnected() ? "Yes" : "No";
    }

    return QVariant();
}

QVariant ActionsModel::VisibilityItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return QVariant();

        case Qt::ToolTipRole:
            return QString("%1 %2").arg(getAction()->text(), getAction()->mayPublish(gui::WidgetAction::ConnectionContextFlag::Gui) ? "is visible" : "is hidden");

        case Qt::DecorationRole:
            return Application::getIconFont("FontAwesome").getIcon("eye");

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        default:
            break;
    }

    return QVariant();
}

ActionsModel::ConnectionPermissionItem::ConnectionPermissionItem(gui::WidgetAction* action, const WidgetAction::ConnectionPermissionFlag& connectionPermissionFlag) :
    Item(action, true),
    _connectionPermissionFlag(connectionPermissionFlag)
{
}

QVariant ActionsModel::ConnectionPermissionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return QVariant();

        case Qt::ToolTipRole:
        {
            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->mayPublish(gui::WidgetAction::ConnectionContextFlag::Gui) ? "may be published from the GUI" : "may not be published from the GUI");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->mayPublish(gui::WidgetAction::ConnectionContextFlag::Gui) ? "may be connected to a public action from the GUI" : "may be connected to a public action from the GUI");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->mayPublish(gui::WidgetAction::ConnectionContextFlag::Gui) ? "may be disconnected from a public action from the GUI" : "may be disconnected from a public action from the GUI");

            break;
        }

        case Qt::DecorationRole:
        {
            auto& fa = Application::getIconFont("FontAwesome");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui)
                return fa.getIcon("cloud-upload-alt");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui)
                return fa.getIcon("link");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui)
                return fa.getIcon("unlink");

            break;
        }

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        default:
            break;
    }

    return QVariant();
}
    
QVariant ActionsModel::SortIndexItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant(QString::number(data(Qt::EditRole).toInt()));

        case Qt::EditRole:
            return getAction()->getSortIndex();

        case Qt::ToolTipRole:
            return QString("Sort index: %1").arg(QString::number(data(Qt::EditRole).toInt()));

        default:
            break;
    }

    return QVariant();
}

ActionsModel::Row::Row(gui::WidgetAction* action) :
    QList<QStandardItem*>()
{
    append(new NameItem(action));
    append(new IdItem(action));
    append(new TypeItem(action));
    append(new ScopeItem(action));
    append(new IsConnectedItem(action));
    append(new VisibilityItem(action));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::PublishViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::ConnectViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::DisconnectViaGui));
    append(new SortIndexItem(action, true));
}

QMap<ActionsModel::Column, ActionsModel::ColumHeaderInfo> ActionsModel::columnInfo = QMap<ActionsModel::Column, ActionsModel::ColumHeaderInfo>({
    { ActionsModel::Column::Name, { "Name" , "Name", "Name of the parameter" } },
    { ActionsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the parameter" } },
    { ActionsModel::Column::Type, { "Type",  "Type", "Type of parameter" } },
    { ActionsModel::Column::Scope, { "Scope",  "Scope", "Scope of the parameter (whether the parameter is public or private)" } },
    { ActionsModel::Column::IsConnected, { "Connected", "Connected", "Whether the parameter is connected or not" } },
    { ActionsModel::Column::Visible, { "", "Visible", "Whether the parameter is visible in the GUI" } },
    { ActionsModel::Column::MayPublish, { "", "May Publish", "Whether the parameter may be published" } },
    { ActionsModel::Column::MayConnect, { "", "May Connect", "Whether the parameter may connect to a parameter action" } },
    { ActionsModel::Column::MayDisconnect, { "", "May Disconnect", "Whether the parameter may disconnect from a public parameter" } },
    { ActionsModel::Column::SortIndex, { "", "Sort Index", "The sorting index of the parameter (its relative position in parameter groups)" } }
});

/*
ActionsModel::Item::Item(ActionsModel* actionsModel, gui::WidgetAction* widgetAction, const ActionsModel::Column& column) :
    QStandardItem(),
    _actionsModel(actionsModel),
    _widgetAction(widgetAction),
    _column(column)
{
    Q_ASSERT(_widgetAction != nullptr);

    if (_widgetAction == nullptr)
        return;

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
            
            connect(_widgetAction, &WidgetAction::changed, this, [this]() -> void {
                emitDataChanged();
            });
            
            //connect(_widgetAction, &WidgetAction::scopeChanged, this, [this]() -> void {
            //    if (_widgetAction->isPublic())
            //        setData(_widgetAction->text(), Qt::EditRole);
            //    
            //    //setCheckState(_widgetAction->isEnabled() ? Qt::Checked : Qt::Unchecked);
            //    //setCheckable(_widgetAction->isPublic());
            //    setEditable(_widgetAction->isPublic());
            //});

            connect(_widgetAction, &WidgetAction::actionConnected, this, [this, actionsModel](WidgetAction* action) -> void {
                const QList<QStandardItem*> row {
                    new Item(actionsModel, action, Column::Name),
                    new Item(actionsModel, action, Column::ID),
                    new Item(actionsModel, action, Column::Type),
                    new Item(actionsModel, action, Column::Scope),
                    new Item(actionsModel, action, Column::IsConnected),
                    new Item(actionsModel, action, Column::Visible),
                    new Item(actionsModel, action, Column::MayPublish),
                    new Item(actionsModel, action, Column::MayConnect),
                    new Item(actionsModel, action, Column::MayDisconnect),
                    new Item(actionsModel, action, Column::SortIndex)
                };

                //for (auto item : row)
                //    item->setEditable(false);

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
            connect(_widgetAction, &WidgetAction::idChanged, this, [this]() -> void {
                emitDataChanged();
            });

            break;
        }

        case Column::Type:
        {
            QTimer::singleShot(10, this, [this]() -> void {
                emitDataChanged();
                _actionsModel->addActionType(data(Qt::DisplayRole).toString());
            });

            break;
        }

        case Column::Scope:
        {
            connect(_widgetAction, &WidgetAction::scopeChanged, this, [this]() -> void {
                //setEditable(_widgetAction->isPublic());
                emitDataChanged();
            });

            break;
        }

        case Column::IsConnected:
        {
            connect(_widgetAction, &WidgetAction::isConnectedChanged, this, [this]() -> void {
                emitDataChanged();
            });

            break;
        }

        default:
            break;
    }
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
*/

ActionsModel::ActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent),
    _actions(),
    _actionTypes(),
    add(0),
    remove(0)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    /*
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
    */
}

void ActionsModel::addAction(WidgetAction* action)
{
#ifdef ACTIONS_MODEL_VERBOSE
    qDebug() << __FUNCTION__ << action->getId();
#endif

    _actions << action;

    const auto row          = Row(action);
    const auto parentAction = action->getParentWidgetAction();

    if (parentAction) {
        const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, parentAction->getId(), 1, Qt::MatchFlag::MatchRecursive);

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
        removeRow(persistentMatch.row(), persistentMatch.parent());
}

WidgetActions ActionsModel::getActions() const
{
    WidgetActions actions;

    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, "*", -1, Qt::MatchFlag::MatchRecursive | Qt::MatchFlag::MatchWildcard);

    //qDebug() << "*Number of actions:" << matches.count() << _actions.count();

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










//#include "ActionHierarchyModel.h"
//#include "ActionHierarchyModelItem.h"
//
//#include <QDebug>
//
//using namespace hdps::gui;
//
//namespace hdps
//{
//
//    ActionHierarchyModel::ActionHierarchyModel(QObject* parent, WidgetAction* rootAction) :
//        QAbstractItemModel(parent),
//        _rootItem(new ActionHierarchyModelItem(rootAction, nullptr))
//    {
//    }
//
//    ActionHierarchyModel::~ActionHierarchyModel()
//    {
//        delete _rootItem;
//    }
//
//    QVariant ActionHierarchyModel::data(const QModelIndex& index, int role) const
//    {
//        if (!index.isValid())
//            return QVariant();
//
//        auto item = static_cast<ActionHierarchyModelItem*>(index.internalPointer());
//
//        return item->getDataAtColumn(index.column(), role);
//    }
//
//    bool ActionHierarchyModel::setData(const QModelIndex& index, const QVariant& value, int role /*= Qt::EditRole*/)
//    {
//        if (!index.isValid())
//            return false;
//
//        auto actionHierarchyModelItem = static_cast<ActionHierarchyModelItem*>((void*)index.internalPointer());
//
//        QModelIndexList childIndices;
//
//        const auto nameIndex = index.siblingAtColumn(ActionHierarchyModelItem::Column::Name);
//
//        for (int rowIndex = 0; rowIndex < rowCount(nameIndex); ++rowIndex)
//            childIndices << this->index(rowIndex, index.column(), nameIndex);
//
//        auto action = actionHierarchyModelItem->getAction();
//
//        switch (role) {
//        case Qt::CheckStateRole:
//        {
//            switch (index.column()) {
//            case ActionHierarchyModelItem::Column::Name:
//            {
//                action->setEnabled(value.toBool());
//
//                for (const auto& childIndex : childIndices)
//                    setData(childIndex, value.toBool(), Qt::CheckStateRole);
//
//                break;
//            }
//
//            case ActionHierarchyModelItem::Column::Visible:
//            {
//                action->setVisible(value.toBool());
//
//                for (const auto& childIndex : childIndices)
//                    setData(childIndex, value.toBool(), Qt::CheckStateRole);
//
//                break;
//            }
//
//            case ActionHierarchyModelItem::Column::MayPublish:
//            {
//                action->setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::PublishViaGui, !value.toBool());
//
//                for (const auto& childIndex : childIndices)
//                    setData(childIndex, value.toBool(), Qt::CheckStateRole);
//
//                break;
//            }
//
//            case ActionHierarchyModelItem::Column::MayConnect:
//            {
//                action->setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::ConnectViaGui, !value.toBool());
//
//                for (const auto& childIndex : childIndices)
//                    setData(childIndex, value.toBool(), Qt::CheckStateRole);
//
//                break;
//            }
//
//            case ActionHierarchyModelItem::Column::MayDisconnect:
//            {
//                action->setConnectionPermissionsFlag(WidgetAction::ConnectionPermissionFlag::DisconnectViaGui, !value.toBool());
//
//                for (const auto& childIndex : childIndices)
//                    setData(childIndex, value.toBool(), Qt::CheckStateRole);
//
//                break;
//            }
//
//            default:
//                break;
//            }
//
//            break;
//        }
//        }
//
//        emit dataChanged(index, index);
//
//        return true;
//    }
//
//    QModelIndex ActionHierarchyModel::index(int row, int column, const QModelIndex& parent) const
//    {
//        if (!hasIndex(row, column, parent))
//            return QModelIndex();
//
//        ActionHierarchyModelItem* parentItem;
//
//        if (!parent.isValid())
//            parentItem = _rootItem;
//        else
//            parentItem = static_cast<ActionHierarchyModelItem*>(parent.internalPointer());
//
//        auto childItem = parentItem->getChild(row);
//
//        if (childItem)
//            return createIndex(row, column, childItem);
//
//        return QModelIndex();
//    }
//
//    QModelIndex ActionHierarchyModel::parent(const QModelIndex& index) const
//    {
//        if (!index.isValid())
//            return QModelIndex();
//
//        auto childItem = static_cast<ActionHierarchyModelItem*>(index.internalPointer());
//        auto parentItem = childItem->getParent();
//
//        if (parentItem == nullptr || parentItem == _rootItem)
//            return QModelIndex();
//
//        return createIndex(parentItem->row(), 0, parentItem);
//    }
//
//    int ActionHierarchyModel::rowCount(const QModelIndex& parent) const
//    {
//        ActionHierarchyModelItem* parentItem;
//
//        if (parent.column() > 0)
//            return 0;
//
//        if (!parent.isValid())
//            parentItem = _rootItem;
//        else
//            parentItem = static_cast<ActionHierarchyModelItem*>(parent.internalPointer());
//
//        return parentItem->getNumChildren();
//    }
//
//    int ActionHierarchyModel::columnCount(const QModelIndex& parent) const
//    {
//        if (parent.isValid())
//            return static_cast<ActionHierarchyModelItem*>(parent.internalPointer())->getNumColumns();
//
//        return _rootItem->getNumColumns();
//    }
//
//    ActionHierarchyModelItem* ActionHierarchyModel::getItem(const QModelIndex& index, int role) const
//    {
//        if (!index.isValid())
//            return nullptr;
//
//        if (role != Qt::DisplayRole)
//            return nullptr;
//
//        return static_cast<ActionHierarchyModelItem*>(index.internalPointer());
//    }
//
//    Qt::ItemFlags ActionHierarchyModel::flags(const QModelIndex& index) const
//    {
//        if (!index.isValid())
//            return Qt::NoItemFlags;
//
//        auto itemFlags = Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
//
//        auto action = static_cast<ActionHierarchyModelItem*>(index.internalPointer())->getAction();
//
//        //if (!action->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::VisibleInMenu))
//        //    itemFlags &= ~Qt::ItemIsEnabled;
//
//        switch (index.column())
//        {
//        case ActionHierarchyModelItem::Column::Name:
//            itemFlags |= Qt::ItemIsUserCheckable;
//            break;
//
//        case ActionHierarchyModelItem::Column::Visible:
//        case ActionHierarchyModelItem::Column::MayPublish:
//        case ActionHierarchyModelItem::Column::MayConnect:
//        case ActionHierarchyModelItem::Column::MayDisconnect:
//            break;
//
//        default:
//            break;
//        }
//
//        return itemFlags;
//    }
//
//    QVariant ActionHierarchyModel::headerData(int section, Qt::Orientation orientation, int role) const
//    {
//        if (orientation == Qt::Horizontal) {
//            switch (role) {
//            case Qt::DisplayRole:
//            {
//                switch (static_cast<ActionHierarchyModelItem::Column>(section))
//                {
//                case ActionHierarchyModelItem::Column::Name:
//                    return "Name";
//
//                case ActionHierarchyModelItem::Column::Visible:
//                    return "";
//
//                case ActionHierarchyModelItem::Column::MayPublish:
//                    return "";
//
//                case ActionHierarchyModelItem::Column::MayConnect:
//                    return "";
//
//                case ActionHierarchyModelItem::Column::MayDisconnect:
//                    return "";
//
//                default:
//                    break;
//                }
//
//                break;
//            }
//
//            case Qt::EditRole:
//            {
//                switch (static_cast<ActionHierarchyModelItem::Column>(section))
//                {
//                case ActionHierarchyModelItem::Column::Name:
//                    return "Name";
//
//                case ActionHierarchyModelItem::Column::Visible:
//                    return "Visible";
//
//                case ActionHierarchyModelItem::Column::MayPublish:
//                    return "May publish";
//
//                case ActionHierarchyModelItem::Column::MayConnect:
//                    return "May connect";
//
//                case ActionHierarchyModelItem::Column::MayDisconnect:
//                    return "May disconnect";
//
//                default:
//                    break;
//                }
//
//                break;
//            }
//
//            case Qt::ToolTipRole:
//            {
//                switch (static_cast<ActionHierarchyModelItem::Column>(section))
//                {
//                case ActionHierarchyModelItem::Column::Name:
//                    return "Name of the action";
//
//                case ActionHierarchyModelItem::Column::Visible:
//                    return "Whether the action is visible or not";
//
//                case ActionHierarchyModelItem::Column::MayPublish:
//                    return "Whether the action may be published or not";
//
//                case ActionHierarchyModelItem::Column::MayConnect:
//                    return "Whether the action may connect to a public action or not";
//
//                case ActionHierarchyModelItem::Column::MayDisconnect:
//                    return "Whether the action may disconnect from a public action or not";
//
//                default:
//                    break;
//                }
//
//                break;
//            }
//
//            default:
//                break;
//            }
//        }
//
//        return QVariant();
//    }
//
//    QModelIndexList ActionHierarchyModel::fetchModelIndices(QModelIndex modelIndex /*= QModelIndex()*/, bool childrenOnly /*= false*/) const
//    {
//        QModelIndexList modelIndexList;
//
//        if (!modelIndex.isValid())
//            return modelIndexList;
//
//        if (!childrenOnly)
//            modelIndexList << modelIndex;
//
//        for (int rowIndex = 0; rowIndex < rowCount(modelIndex); ++rowIndex) {
//            QModelIndex index = this->index(rowIndex, 0, modelIndex);
//
//            modelIndexList << index;
//
//            if (hasChildren(index))
//                modelIndexList << fetchModelIndices(index, false);
//        }
//
//        return modelIndexList;
//    }
//
//}
