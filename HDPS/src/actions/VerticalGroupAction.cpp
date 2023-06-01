#include "VerticalGroupAction.h"

namespace hdps::gui {

VerticalGroupAction::VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded)
{
    setDefaultWidgetFlags(GroupAction::Vertical);
}

}