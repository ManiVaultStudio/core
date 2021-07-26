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

bool WidgetActionGroup::isExpanded() const
{
    return _expanded;
}

}
}