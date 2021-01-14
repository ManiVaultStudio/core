#include "WidgetStateMixin.h"

WidgetStateMixin::WidgetStateMixin(const QString& title) :
    _state(State::Undefined),
    _title(title)
{
}