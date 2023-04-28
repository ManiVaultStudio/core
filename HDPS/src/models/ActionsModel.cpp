#include "ActionsModel.h"
#include "AbstractActionsManager.h"

#include "actions/WidgetAction.h"

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
    QStandardItem(),
    QObject(),
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
    return getAction()->text();
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

    return {};
}

QVariant ActionsModel::ScopeItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return static_cast<int>(getAction()->getScope());

        case Qt::DisplayRole:
            return WidgetAction::scopeNames[getAction()->getScope()];
    }

    return {};
}

ActionsModel::VisibilityItem::VisibilityItem(gui::WidgetAction* action) :
    Item(action, false)
{
    setEditable(false);

    connect(action, &WidgetAction::forceHiddenChanged, this, [this](bool forceHidden) -> void {
        emitDataChanged();
    });
}

QVariant ActionsModel::VisibilityItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->getForceHidden();

        case Qt::ToolTipRole:
            return QString("%1 %2").arg(getAction()->text(), getAction()->getForceHidden() ? "is hidden" : "is not hidden");

        case Qt::DecorationRole:
            return Application::getIconFont("FontAwesome").getIcon("eye-slash");

        case Qt::TextAlignmentRole:
            return Qt::AlignCenter;

        default:
            break;
    }

    return {};
}

void ActionsModel::VisibilityItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setForceHidden(value.toBool());
}

ActionsModel::ConnectionPermissionItem::ConnectionPermissionItem(gui::WidgetAction* action, const WidgetAction::ConnectionPermissionFlag& connectionPermissionFlag) :
    Item(action, true),
    _connectionPermissionFlag(connectionPermissionFlag)
{
    connect(action, &WidgetAction::connectionPermissionsChanged, this, [this](std::int32_t connectionPermissions) -> void {
        emitDataChanged();
    });
}

QVariant ActionsModel::ConnectionPermissionItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role)
    {
        case Qt::DisplayRole:
            return QVariant();

        case Qt::EditRole:
            return getAction()->isConnectionPermissionFlagSet(_connectionPermissionFlag);

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

    return {};
}

void ActionsModel::ConnectionPermissionItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    if (role == Qt::EditRole)
        getAction()->setConnectionPermissionsFlag(_connectionPermissionFlag, !value.toBool());
}

ActionsModel::SortIndexItem::SortIndexItem(gui::WidgetAction* action) :
    Item(action, true)
{
    setTextAlignment(Qt::AlignRight);

    connect(action, &WidgetAction::sortIndexChanged, this, [this](std::int32_t sortIndex) -> void {
        emitDataChanged();
    });
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

    return {};
}

void ActionsModel::SortIndexItem::setData(const QVariant& value, int role /* = Qt::UserRole + 1 */)
{
    getAction()->setSortIndex(value.toInt());
}

QVariant ActionsModel::ParentActionIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->getParentAction() ? getAction()->getParentAction()->getId() : "";
    }

    return {};
}

QVariant ActionsModel::IsConnectedItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
            return getAction()->isConnected();

        case Qt::DisplayRole:
            return getAction()->isConnected() ? "Yes" : "No";
    }

    return {};
}

QVariant ActionsModel::PublicActionIdItem::data(int role /*= Qt::UserRole + 1*/) const
{
    switch (role) {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return getAction()->isConnected() ? getAction()->getPublicAction()->getId() : "";
    }

    return {};
}

ActionsModel::Row::Row(gui::WidgetAction* action) :
    QList<QStandardItem*>()
{
    append(new NameItem(action));
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
}

QMap<ActionsModel::Column, ActionsModel::ColumHeaderInfo> ActionsModel::columnInfo = QMap<ActionsModel::Column, ActionsModel::ColumHeaderInfo>({
    { ActionsModel::Column::Name, { "Name" , "Name", "Name of the parameter" } },
    { ActionsModel::Column::ID, { "ID",  "ID", "Globally unique identifier of the parameter" } },
    { ActionsModel::Column::Type, { "Type",  "Type", "Type of parameter" } },
    { ActionsModel::Column::Scope, { "Scope",  "Scope", "Scope of the parameter (whether the parameter is public or private)" } },
    { ActionsModel::Column::Visible, { "", "Visible", "Whether the parameter is visible in the GUI" } },
    { ActionsModel::Column::MayPublish, { "", "May Publish", "Whether the parameter may be published" } },
    { ActionsModel::Column::MayConnect, { "", "May Connect", "Whether the parameter may connect to a public parameter" } },
    { ActionsModel::Column::MayDisconnect, { "", "May Disconnect", "Whether the parameter may disconnect from a public parameter" } },
    { ActionsModel::Column::SortIndex, { "Sort Index", "Sort Index", "The sorting index of the parameter (its relative position in parameter groups)" } },
    { ActionsModel::Column::ParentActionId, { "Parent ID", "Parent ID", "The identifier of the parent parameter (if not a top-level parameter)" } },
    { ActionsModel::Column::IsConnected, { "Connected", "Connected", "Whether the parameter is connected or not" } },
    { ActionsModel::Column::PublicActionID, { "Public Parameter ID", "Public Parameter ID", "The identifier of the public parameter with which the parameter is connected" } }
});

ActionsModel::ActionsModel(QObject* parent /*= nullptr*/) :
    QStandardItemModel(parent)
{
    setColumnCount(static_cast<int>(Column::Count));

    for (auto column : columnInfo.keys())
        setHorizontalHeaderItem(static_cast<int>(column), new HeaderItem(columnInfo[column]));

    connect(&hdps::actions(), &AbstractActionsManager::actionAdded, this, &ActionsModel::addAction);
    connect(&hdps::actions(), &AbstractActionsManager::actionAboutToBeRemoved, this, &ActionsModel::removeAction);
}

Qt::ItemFlags ActionsModel::flags(const QModelIndex& index) const
{
    if (index.column() != static_cast<int>(ActionsModel::Column::SortIndex))
        return  QStandardItemModel::flags(index) & ~Qt::ItemIsEditable;
    
    return  QStandardItemModel::flags(index) | Qt::ItemIsEditable;
}

WidgetAction* ActionsModel::getAction(std::int32_t rowIndex)
{
    return static_cast<Item*>(item(rowIndex, 0))->getAction();
}

WidgetAction* ActionsModel::getAction(const QModelIndex& index)
{
    return static_cast<Item*>(itemFromIndex(index))->getAction();
}

QModelIndex ActionsModel::getActionIndex(const gui::WidgetAction* action) const
{
    const auto matches = match(index(0, static_cast<int>(Column::ID), QModelIndex()), Qt::EditRole, action->getId(), 1, Qt::MatchFlag::MatchRecursive);

    if (matches.count() == 1)
        return matches.first();

    return {};
}

}
