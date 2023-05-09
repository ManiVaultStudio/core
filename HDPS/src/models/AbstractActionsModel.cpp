#include "AbstractActionsModel.h"
#include "AbstractActionsManager.h"

#include "actions/WidgetAction.h"

#include "models/ActionsListModel.h"

using namespace hdps::gui;

#ifdef _DEBUG
    //#define ABSTRACT_ACTIONS_MODEL_VERBOSE
#endif

namespace hdps
{

AbstractActionsModel::HeaderItem::HeaderItem(const ColumHeaderInfo& columHeaderInfo) :
    QStandardItem(),
    _columHeaderInfo(columHeaderInfo)
{
}

QVariant AbstractActionsModel::HeaderItem::data(int role /*= Qt::UserRole + 1*/) const
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

AbstractActionsModel::Item::Item(gui::WidgetAction* action, bool editable /*= false*/) :
    QStandardItem(),
    QObject(),
    _action(action)
{
    Q_ASSERT(_action != nullptr);

    setEditable(editable);
}

QPointer<hdps::gui::WidgetAction> AbstractActionsModel::Item::getAction() const
{
    return _action;
}

AbstractActionsModel::NameItem::NameItem(gui::WidgetAction* action) :
    Item(action)
{
    setCheckable(true);
    setCheckState(getAction()->isEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

    connect(getAction(), &WidgetAction::changed, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::NameItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->text();

        case Qt::CheckStateRole:
            return getAction()->isEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;

        case Qt::ToolTipRole:
            return data(Qt::DisplayRole).toString() + " is " + QString(getAction()->isEnabled() ? "enabled" : "disabled");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::CheckStateRole)
        getAction()->setEnabled(value.toBool());
    else
        Item::setData(value, role);
}

QVariant AbstractActionsModel::PathItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->isPrivate() ? getAction()->getPath() : "";

        case Qt::ToolTipRole:
            return getAction()->isPrivate() ? "Parameter is located in: " + data(Qt::DisplayRole).toString() : "";

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::IdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->getId();

        case Qt::ToolTipRole:
            return "Parameter globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::TypeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->getTypeString();

        case Qt::DisplayRole:
            return getAction()->getTypeString(true);

        case Qt::ToolTipRole:
            return "Parameter type: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::ScopeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getAction()->getScope());

        case Qt::DisplayRole:
            return WidgetAction::scopeNames[getAction()->getScope()];

        case Qt::ToolTipRole:
            return "Parameter is: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractActionsModel::VisibilityItem::VisibilityItem(gui::WidgetAction* action) :
    Item(action, false)
{
    connect(getAction(), &WidgetAction::forceHiddenChanged, this, [this](bool forceHidden) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::VisibilityItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->getForceHidden();

        case Qt::DecorationRole:
            return Application::getIconFont("FontAwesome").getIcon("eye-slash");

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::ToolTipRole:
            return QString("%1 %2").arg(getAction()->text(), getAction()->getForceHidden() ? "is hidden" : "is not hidden");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::VisibilityItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setForceHidden(value.toBool());
    else
        Item::setData(value, role);
}

AbstractActionsModel::ConnectionPermissionItem::ConnectionPermissionItem(gui::WidgetAction* action, const WidgetAction::ConnectionPermissionFlag& connectionPermissionFlag) :
    Item(action, true),
    _connectionPermissionFlag(connectionPermissionFlag)
{
    connect(action, &WidgetAction::connectionPermissionsChanged, this, [this](std::int32_t connectionPermissions) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::ConnectionPermissionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->isConnectionPermissionFlagSet(_connectionPermissionFlag);

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

        case Qt::ToolTipRole:
        {
            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->isConnectionPermissionFlagSet(gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui) ? "may be published from the GUI" : "may not be published from the GUI");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->isConnectionPermissionFlagSet(gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui) ? "may be connected to a public parameter from the GUI" : "may not be connected to a public parameter from the GUI");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui)
                return QString("%1 %2").arg(getAction()->text(), getAction()->isConnectionPermissionFlagSet(gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui) ? "may be disconnected from a public parameter from the GUI" : "may not be disconnected from a public parameter from the GUI");

            break;
        }

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::ConnectionPermissionItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setConnectionPermissionsFlag(_connectionPermissionFlag, !value.toBool());
    else
        Item::setData(value, role);
}

AbstractActionsModel::SortIndexItem::SortIndexItem(gui::WidgetAction* action) :
    Item(action, true)
{
    setTextAlignment(Qt::AlignRight);

    connect(action, &WidgetAction::sortIndexChanged, this, [this](std::int32_t sortIndex) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::SortIndexItem::data(int role /*= Qt::UserRole + 1*/) const
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

    return Item::data(role);
}

void AbstractActionsModel::SortIndexItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setSortIndex(value.toInt());
    else
        Item::setData(value, role);
}

QVariant AbstractActionsModel::ParentActionIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->getParentAction() ? getAction()->getParentAction()->getId() : "";

        case Qt::ToolTipRole:
            return QString("Parent parameter globally unique identifier: %1").arg(data(Qt::DisplayRole).toString());

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::IsConnectedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->isConnected();

        case Qt::DisplayRole:
            return getAction()->isConnected() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Parameter " + QString(getAction()->isConnected() ? "is connected" : "is not connected");

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::PublicActionIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->isConnected() ? getAction()->getPublicAction()->getId() : "";

        case Qt::ToolTipRole:
            return "Public parameter globally unique identifier: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::IsRootItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->isRoot();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Root parameter: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::IsLeafItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->isLeaf();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Leaf parameter: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractActionsModel::Row::Row(gui::WidgetAction* action) :
    QList<QStandardItem*>()
{
    append(new NameItem(action));
    append(new PathItem(action));
    append(new IdItem(action));
    append(new TypeItem(action));
    append(new ScopeItem(action));
    append(new VisibilityItem(action));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::PublishViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::ConnectViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::DisconnectViaGui));
    append(new SortIndexItem(action));
    append(new ParentActionIdItem(action));
    append(new IsConnectedItem(action));
    append(new PublicActionIdItem(action));
    append(new IsRootItem(action));
    append(new IsLeafItem(action));
}

QMap<AbstractActionsModel::Column, AbstractActionsModel::ColumHeaderInfo> AbstractActionsModel::columnInfo = QMap<AbstractActionsModel::Column, AbstractActionsModel::ColumHeaderInfo>({
    { AbstractActionsModel::Column::Name, { "Name" , "Name", "Name of the parameter" } },
    { AbstractActionsModel::Column::Location, { "Location" , "Location", "Where the parameter is located in the user interface" } },
    { AbstractActionsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the parameter" } },
    { AbstractActionsModel::Column::Type, { "Type",  "Type", "Type of parameter" } },
    { AbstractActionsModel::Column::Scope, { "Scope",  "Scope", "Scope of the parameter (whether the parameter is public or private)" } },
    { AbstractActionsModel::Column::Visible, { "", "Visible", "Whether the parameter is visible in the GUI" } },
    { AbstractActionsModel::Column::MayPublish, { "", "May Publish", "Whether the parameter may be published" } },
    { AbstractActionsModel::Column::MayConnect, { "", "May Connect", "Whether the parameter may connect to a public parameter" } },
    { AbstractActionsModel::Column::MayDisconnect, { "", "May Disconnect", "Whether the parameter may disconnect from a public parameter" } },
    { AbstractActionsModel::Column::SortIndex, { "Sort Index", "Sort Index", "The sorting index of the parameter (its relative position in parameter groups)" } },
    { AbstractActionsModel::Column::ParentActionId, { "Parent ID", "Parent ID", "The identifier of the parent parameter (if not a top-level parameter)" } },
    { AbstractActionsModel::Column::IsConnected, { "Connected", "Connected", "Whether the parameter is connected or not" } },
    { AbstractActionsModel::Column::PublicActionID, { "Public Parameter ID", "Public Parameter ID", "The identifier of the public parameter with which the parameter is connected" } },
    { AbstractActionsModel::Column::IsRoot, { "Root", "Root", "Whether the parameter is located at the root of the hierarchy" } },
    { AbstractActionsModel::Column::IsLeaf, { "Leaf", "Leaf", "Whether the parameter is a leaf or not" } }
});

AbstractActionsModel::AbstractActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&hdps::actions(), &AbstractActionsManager::actionAdded, this, &AbstractActionsModel::actionAddedToManager);
    connect(&hdps::actions(), &AbstractActionsManager::actionAboutToBeRemoved, this, &AbstractActionsModel::actionAboutToBeRemovedFromManager);
    connect(&hdps::actions(), &AbstractActionsManager::publicActionAdded, this, &AbstractActionsModel::publicActionAddedToManager);
    connect(&hdps::actions(), &AbstractActionsManager::publicActionAboutToBeRemoved, this, &AbstractActionsModel::publicActionAboutToBeRemovedFromManager);
}

Qt::ItemFlags AbstractActionsModel::flags(const QModelIndex& index) const
{
    if (index.column() == static_cast<int>(AbstractActionsModel::Column::Name))
        return (QStandardItemModel::flags(index) & ~Qt::ItemIsEditable) | Qt::ItemIsUserCheckable;
    
    return  QStandardItemModel::flags(index);
}

WidgetAction* AbstractActionsModel::getAction(const QModelIndex& index)
{
    return static_cast<Item*>(itemFromIndex(index))->getAction();
}

WidgetAction* AbstractActionsModel::getAction(std::int32_t rowIndex)
{
    return static_cast<Item*>(item(rowIndex, 0))->getAction();
}

QModelIndex AbstractActionsModel::getActionIndex(const gui::WidgetAction* action, const Column& column /** = Column::Name */) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, action->getId(), 1, Qt::MatchFlag::MatchRecursive);

    if (matches.count() == 1)
        return matches.first().siblingAtColumn(static_cast<int>(column));

    return {};
}

QStandardItem* AbstractActionsModel::getActionItem(const gui::WidgetAction* action) const
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return nullptr;

    const auto parentActionIndex = getActionIndex(action);

    if (!parentActionIndex.isValid())
        return nullptr;

    return itemFromIndex(parentActionIndex);
}

}
