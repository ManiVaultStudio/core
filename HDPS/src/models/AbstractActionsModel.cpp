#include "AbstractActionsModel.h"
#include "AbstractActionsManager.h"

#include "actions/WidgetAction.h"
#include "actions/WidgetActionMimeData.h"

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
    setDropEnabled(true);
}

QPointer<hdps::gui::WidgetAction> AbstractActionsModel::Item::getAction() const
{
    return _action;
}

AbstractActionsModel::NameItem::NameItem(gui::WidgetAction* action) :
    Item(action)
{
    setEditable(action->isPublic());

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

        case Qt::ToolTipRole:
            return QString("Parameter name: %1").arg(data(Qt::DisplayRole).toString());

        //case Qt::CheckStateRole:
        //    return getAction()->isEnabled() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;

        //case Qt::ToolTipRole:
        //    return data(Qt::DisplayRole).toString() + " is " + QString(getAction()->isEnabled() ? "enabled" : "disabled");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::NameItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getAction()->setText(value.toString());
            break;

        default:
            Item::setData(value, role);
    }
}

QVariant AbstractActionsModel::LocationItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->getLocation();

        case Qt::ToolTipRole:
            return "Parameter is located in: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractActionsModel::IdItem::IdItem(gui::WidgetAction* action) :
    Item(action)
{
    connect(getAction(), &WidgetAction::idChanged, this, [this](const QString& id) -> void {
        emitDataChanged();
    });
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

AbstractActionsModel::ForceDisabledItem::ForceDisabledItem(gui::WidgetAction* action) :
    Item(action, false)
{
    connect(getAction(), &WidgetAction::forceDisabledChanged, this, [this]() -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::ForceDisabledItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

    const auto& fontAwesome = Application::getIconFont("FontAwesome");

    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->getForceDisabled();

        case Qt::DecorationRole:
            return fontAwesome.getIcon("lock");

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        case Qt::ToolTipRole:
            return QString("%1 %2 force disabled").arg(getAction()->text(), data(Qt::EditRole).toBool() ? "is" : "is not");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::ForceDisabledItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    switch (role) {
        case Qt::EditRole:
            getAction()->setForceDisabled(value.toBool());
            break;

        default:
            Item::setData(value, role);
    }
}

AbstractActionsModel::ForceHiddenItem::ForceHiddenItem(gui::WidgetAction* action) :
    Item(action, false)
{
    connect(getAction(), &WidgetAction::forceHiddenChanged, this, [this](bool forceHidden) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::ForceHiddenItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

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
            return QString("%1 %2 force hidden").arg(getAction()->text(), getAction()->getForceHidden() ? "is" : "is not");

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::ForceHiddenItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
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
    setEnabled(!getAction()->isConnectionPermissionFlagSet(WidgetAction::ConnectionPermissionFlag::ForceNone));

    connect(action, &WidgetAction::connectionPermissionsChanged, this, [this](std::int32_t connectionPermissions) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::ConnectionPermissionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->isConnectionPermissionFlagSet(_connectionPermissionFlag);

        case Qt::DecorationRole:
        {
            if (getAction()->isConnectionPermissionFlagSet(WidgetAction::ConnectionPermissionFlag::ForceNone))
                break;

            auto& fontAwesome = Application::getIconFont("FontAwesome");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::PublishViaGui)
                return fontAwesome.getIcon("cloud-upload-alt");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::ConnectViaGui)
                return fontAwesome.getIcon("link");

            if (_connectionPermissionFlag == gui::WidgetAction::ConnectionPermissionFlag::DisconnectViaGui)
                return fontAwesome.getIcon("unlink");

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
    if (role == Qt::EditRole && !getAction()->isConnectionPermissionFlagSet(WidgetAction::ConnectionPermissionFlag::ForceNone))
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
    if (getAction()->isPublic())
        return {};

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

AbstractActionsModel::StretchItem::StretchItem(gui::WidgetAction* action) :
    Item(action, true)
{
    setTextAlignment(Qt::AlignRight);

    connect(action, &WidgetAction::stretchChanged, this, [this](std::int32_t stretch) -> void {
        emitDataChanged();
    });
}

QVariant AbstractActionsModel::StretchItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant(QString::number(data(Qt::EditRole).toInt()));

        case Qt::EditRole:
            return getAction()->getStretch();

        case Qt::ToolTipRole:
            return QString("Stretch: %1").arg(QString::number(data(Qt::EditRole).toInt()));

        default:
            break;
    }

    return Item::data(role);
}

void AbstractActionsModel::StretchItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setStretch(value.toInt());
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
    if (getAction()->isPublic())
        return {};

    switch (role) {
        case Qt::EditRole:
            return getAction()->isConnected();

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Parameter " + QString(getAction()->isConnected() ? "is connected" : "is not connected");

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::NumberOfConnectedActionsItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPrivate())
        return {};

    switch (role) {
        case Qt::EditRole:
            return getAction()->getConnectedActions().count();

        case Qt::DisplayRole:
            return QString::number(data(Qt::EditRole).toInt());

        case Qt::ToolTipRole:
            return "Number of connected parameters: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

QVariant AbstractActionsModel::PublicActionIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    if (getAction()->isPublic())
        return {};

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

QVariant AbstractActionsModel::InternalUseOnlyItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return (getAction()->getConfiguration() & static_cast<std::int32_t>(WidgetAction::ConfigurationFlag::InternalUseOnly));

        case Qt::DisplayRole:
            return data(Qt::EditRole).toBool() ? "Yes" : "No";

        case Qt::ToolTipRole:
            return "Parameter internal use only: " + data(Qt::DisplayRole).toString();

        default:
            break;
    }

    return Item::data(role);
}

AbstractActionsModel::Row::Row(gui::WidgetAction* action) :
    QList<QStandardItem*>()
{
    append(new NameItem(action));
    append(new LocationItem(action));
    append(new IdItem(action));
    append(new TypeItem(action));
    append(new ScopeItem(action));
    append(new ForceDisabledItem(action));
    append(new ForceHiddenItem(action));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::PublishViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::ConnectViaGui));
    append(new ConnectionPermissionItem(action, WidgetAction::ConnectionPermissionFlag::DisconnectViaGui));
    append(new SortIndexItem(action));
    append(new StretchItem(action));
    append(new ParentActionIdItem(action));
    append(new IsConnectedItem(action));
    append(new NumberOfConnectedActionsItem(action));
    append(new PublicActionIdItem(action));
    append(new IsRootItem(action));
    append(new IsLeafItem(action));
    append(new InternalUseOnlyItem(action));
}

QMap<AbstractActionsModel::Column, AbstractActionsModel::ColumHeaderInfo> AbstractActionsModel::columnInfo = QMap<AbstractActionsModel::Column, AbstractActionsModel::ColumHeaderInfo>({
    { AbstractActionsModel::Column::ForceDisabled, { "" , "Enabled", "Whether the parameter is enabled or not" } },
    { AbstractActionsModel::Column::Name, { "Name" , "Name", "Name of the parameter" } },
    { AbstractActionsModel::Column::Location, { "Location" , "Location", "Where the parameter is located in the user interface" } },
    { AbstractActionsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the parameter" } },
    { AbstractActionsModel::Column::Type, { "Type",  "Type", "Type of parameter" } },
    { AbstractActionsModel::Column::Scope, { "Scope",  "Scope", "Scope of the parameter (whether the parameter is public or private)" } },
    { AbstractActionsModel::Column::ForceDisabled, { "", "Force Disabled", "Whether the parameter is forcibly disabled (regardless of its programmatic enabled setting)" } },
    { AbstractActionsModel::Column::ForceHidden, { "", "Force Hidden", "Whether the parameter is forcibly hidden (regardless of its programmatic visibility setting)" } },
    { AbstractActionsModel::Column::MayPublish, { "", "May Publish", "Whether the parameter may be published" } },
    { AbstractActionsModel::Column::MayConnect, { "", "May Connect", "Whether the parameter may connect to a public parameter" } },
    { AbstractActionsModel::Column::MayDisconnect, { "", "May Disconnect", "Whether the parameter may disconnect from a public parameter" } },
    { AbstractActionsModel::Column::SortIndex, { "Sort Index", "Sort Index", "The sorting index of the parameter (its relative position in parameter groups)" } },
    { AbstractActionsModel::Column::Stretch, { "Stretch", "Stretch", "The parameter stretch in parameter groups" } },
    { AbstractActionsModel::Column::ParentActionId, { "Parent ID", "Parent ID", "The identifier of the parent parameter (if not a top-level parameter)" } },
    { AbstractActionsModel::Column::IsConnected, { "Connected", "Connected", "Whether the parameter is connected or not" } },
    { AbstractActionsModel::Column::NumberOfConnectedActions, { "No. Connected Parameters", "No. Connected Parameters", "The number of connected parameters (in case the parameter is public)" } },
    { AbstractActionsModel::Column::PublicActionID, { "Public Parameter ID", "Public Parameter ID", "The identifier of the public parameter with which the parameter is connected" } },
    { AbstractActionsModel::Column::IsRoot, { "Root", "Root", "Whether the parameter is located at the root of the hierarchy" } },
    { AbstractActionsModel::Column::IsLeaf, { "Leaf", "Leaf", "Whether the parameter is a leaf or not" } },
    { AbstractActionsModel::Column::InternalUseOnly, { "Internal use only", "Internal use only", "Whether the parameter is for internal use only" } }
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
    //if (index.column() == static_cast<int>(AbstractActionsModel::Column::ForceDisabled))
    //    return QStandardItemModel::flags(index) | Qt::ItemIsUserCheckable;
    
    return  QStandardItemModel::flags(index) | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

WidgetAction* AbstractActionsModel::getAction(const QModelIndex& index)
{
    auto actionItem = static_cast<Item*>(itemFromIndex(index));

    if (!actionItem)
        return nullptr;

    return actionItem->getAction();
}

WidgetAction* AbstractActionsModel::getAction(std::int32_t rowIndex)
{
    return static_cast<Item*>(item(rowIndex, 0))->getAction();
}

WidgetAction* AbstractActionsModel::getAction(const QString& name)
{
    const auto matches = match(index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchFlag::MatchRecursive);

    if (matches.isEmpty())
        return nullptr;

    if (matches.count() > 1)
        throw std::runtime_error(QString("Found more than one action with the name: %1").arg(name).toLatin1());

    return getAction(matches.first());
}

QModelIndex AbstractActionsModel::getActionIndex(const gui::WidgetAction* action, const Column& column /** = Column::Name */) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, action->getId(), 1, Qt::MatchFlag::MatchRecursive | Qt::MatchExactly);

    if (matches.count() == 1)
        return matches.first().siblingAtColumn(static_cast<int>(column));

    return {};
}

QStandardItem* AbstractActionsModel::getActionItem(const gui::WidgetAction* action) const
{
    Q_ASSERT(action != nullptr);

    if (action == nullptr)
        return nullptr;

    const auto actionIndex = getActionIndex(action);

    if (!actionIndex.isValid())
        return nullptr;

    return itemFromIndex(actionIndex);
}

Qt::DropActions AbstractActionsModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

Qt::DropActions AbstractActionsModel::supportedDragActions() const
{
    return Qt::CopyAction;
}

QStringList AbstractActionsModel::mimeTypes() const
{
    return { "application/action" };
}

QMimeData* AbstractActionsModel::mimeData(const QModelIndexList& indexes) const
{
    if (indexes.isEmpty())
        return nullptr;

    return new WidgetActionMimeData(static_cast<Item*>(itemFromIndex(indexes.first()))->getAction());
}

bool AbstractActionsModel::canDropMimeData(const QMimeData* mimeData, Qt::DropAction dropAction, int row, int column, const QModelIndex& parent) const
{
    auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(mimeData);

    if (actionMimeData == nullptr)
        return false;

    if (row >= 0 && column != 0)
        return false;

    auto action = const_cast<AbstractActionsModel*>(this)->getAction(row < 0 ? parent : index(row, 0, parent));

    if (action != nullptr) {
        if (action->isPrivate())
            return false;

        if (actionMimeData->getAction()->getTypeString() != action->getTypeString())
            return false;
    }
        
    return true;
}

bool AbstractActionsModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction dropAction, int row, int column, const QModelIndex& parent)
{
    auto actionMimeData = dynamic_cast<const WidgetActionMimeData*>(mimeData);

    if (actionMimeData == nullptr)
        return false;

    if (!parent.isValid()) {
        if (actionMimeData->getAction()->isPrivate() && !actionMimeData->getAction()->isConnected()) {
            actions().publishPrivateAction(actionMimeData->getAction());
            return true;
        }
    }
    else {
        auto action = getAction(row < 0 ? parent : index(row, 0, parent));

        if (action != nullptr)
            actions().connectPrivateActionToPublicAction(actionMimeData->getAction(), action, true);
    }

    return false;
}

}
