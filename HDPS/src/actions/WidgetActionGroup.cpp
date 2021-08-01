#include "WidgetActionGroup.h"

#include <QDebug>
#include <QMenu>
#include <QEvent>
#include <QPainter>

namespace hdps {

namespace gui {

WidgetActionGroup::WidgetActionGroup(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded)
{
}

void WidgetActionGroup::setExpanded(const bool& expanded)
{
    if (expanded == _expanded)
        return;

    if (expanded)
        expand();
    else
        collapse();
}

void WidgetActionGroup::expand()
{
    if (_expanded == true)
        return;

    _expanded = true;

    emit expanded();
}

void WidgetActionGroup::collapse()
{
    if (_expanded == false)
        return;

    _expanded = false;

    emit collapsed();
}

void WidgetActionGroup::toggle()
{
    setExpanded(!isExpanded());
}

bool WidgetActionGroup::isExpanded() const
{
    return _expanded;
}

bool WidgetActionGroup::isCollapsed() const
{
    return !_expanded;
}

}
}