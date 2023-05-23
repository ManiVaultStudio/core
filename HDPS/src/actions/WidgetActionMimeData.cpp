#include "WidgetActionMimeData.h"

namespace hdps::gui {

WidgetActionMimeData::WidgetActionMimeData(WidgetAction* action) :
    QMimeData(),
    _action(action)
{
}

QStringList WidgetActionMimeData::formats() const
{
    return { format() };
}

}