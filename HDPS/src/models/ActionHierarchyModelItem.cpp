#include "ActionHierarchyModelItem.h"
#include "Application.h"

#include <QDebug>

namespace hdps
{

using namespace gui;

ActionHierarchyModelItem::ActionHierarchyModelItem(WidgetAction* action, ActionHierarchyModelItem* parent /*= nullptr*/) :
    QObject(parent),
    _action(action),
    _parent(parent),
    _children()
{
    for (auto child : _action->children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (childWidgetAction == nullptr)
            continue;

        _children.append(new ActionHierarchyModelItem(childWidgetAction, this));
    }
}

ActionHierarchyModelItem::~ActionHierarchyModelItem()
{
    if (_parent)
        _parent->removeChild(this);

    qDeleteAll(_children);
}

ActionHierarchyModelItem* ActionHierarchyModelItem::getParent()
{
    return _parent;
}

void ActionHierarchyModelItem::setParent(ActionHierarchyModelItem* parent)
{
    _parent = parent;
}

ActionHierarchyModelItem* ActionHierarchyModelItem::getChild(const std::int32_t& row)
{
    if (row < 0 || row >= _children.size())
        return nullptr;

    return _children[row];
}

std::int32_t ActionHierarchyModelItem::row() const
{
    if (_parent)
        return _parent->_children.indexOf(const_cast<ActionHierarchyModelItem*>(this));

    return 0;
}

std::int32_t ActionHierarchyModelItem::getNumChildren() const
{
    return _children.count();
}

std::int32_t ActionHierarchyModelItem::getNumColumns() const
{
    return static_cast<std::int32_t>(Column::_end) + 1;
}

QVariant ActionHierarchyModelItem::getDataAtColumn(const std::uint32_t& column, int role /*= Qt::DisplayRole*/) const
{
    
    if (_action == nullptr)
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            const auto editValue = getDataAtColumn(column, Qt::EditRole);

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return editValue;

                case Column::Visible:
                case Column::MayPublish:
                case Column::MayConnect:
                case Column::MayDisconnect:
                    return "";

                default:
                    break;
            }

            break;
        }

        case Qt::EditRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _action->text();

                case Column::Visible:
                    return _action->isVisible();

                case Column::MayPublish:
                    return _action->mayPublish(WidgetAction::Gui);

                case Column::MayConnect:
                    return _action->mayConnect(WidgetAction::Gui);

                case Column::MayDisconnect:
                    return _action->mayDisconnect(WidgetAction::Gui);

                default:
                    break;
            }

            break;
        }

        case Qt::ToolTipRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _action->text();

                case Column::Visible:
                    return QString("%1 is %2").arg(_action->text(), _action->isVisible() ? "visible" : "not visible");

                case Column::MayPublish:
                    return QString("%1 %2").arg(_action->text(), _action->isVisible() ? "may be published" : "may not be published");

                case Column::MayConnect:
                    return QString("%1 %2").arg(_action->text(), _action->isVisible() ? "may be connected to a public action" : "may not be connected to a public action");

                case Column::MayDisconnect:
                    return QString("%1 %2").arg(_action->text(), _action->isVisible() ? "may be disconnected from a public action" : "may not be disconnected from a public action");

                default:
                    break;
            }

            break;
        }

        
        case Qt::CheckStateRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    return _action->isEnabled() ? Qt::Checked : Qt::Unchecked;

                case Column::Visible:
                case Column::MayPublish:
                case Column::MayConnect:
                case Column::MayDisconnect:
                    break;

                default:
                    break;
            }

            break;
        }

        case Qt::DecorationRole:
        {
            auto& fa = Application::getIconFont("FontAwesome");

            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    break;

                case Column::Visible:
                    return fa.getIcon("eye");

                case Column::MayPublish:
                    return fa.getIcon("cloud-upload-alt");

                case Column::MayConnect:
                    return fa.getIcon("link");

                case Column::MayDisconnect:
                    return fa.getIcon("unlink");

                default:
                    break;
            }

            break;
        }

        case Qt::TextAlignmentRole:
        {
            switch (static_cast<Column>(column))
            {
                case Column::Name:
                    break;

                case Column::Visible:
                case Column::MayPublish:
                case Column::MayConnect:
                case Column::MayDisconnect:
                    return Qt::AlignCenter;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    return QVariant();
}

WidgetAction* ActionHierarchyModelItem::getAction()
{
    return _action;
}

void ActionHierarchyModelItem::removeChild(ActionHierarchyModelItem* actionHierarchyModelItem)
{
    _children.removeOne(actionHierarchyModelItem);
}

}
