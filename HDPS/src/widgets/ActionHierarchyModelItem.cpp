#include "ActionHierarchyModelItem.h"

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
                    return "";

                case Column::Linkable:
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

                case Column::Linkable:
                    return true;

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

                case Column::Linkable:
                    return QString("%1 is %2").arg(_action->text(), _action->isVisible() ? "linkable" : "not linkable");

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
                    break;

                case Column::Visible:
                    return _action->isVisible() ? Qt::Checked : Qt::Unchecked;
                
                case Column::Linkable:
                    return false;

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

bool ActionHierarchyModelItem::isVisible() const
{
    return _action->isVisible();
}

void ActionHierarchyModelItem::setVisible(bool visible)
{
    _action->setVisible(visible);
}

void ActionHierarchyModelItem::removeChild(ActionHierarchyModelItem* actionHierarchyModelItem)
{
    _children.removeOne(actionHierarchyModelItem);
}

}
