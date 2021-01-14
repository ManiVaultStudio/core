#include "WidgetStateMixin.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>

WidgetStateMixin::WidgetStateMixin(const QString& title) :
    _state(State::Undefined),
    _title(title)
{
}

void WidgetStateMixin::setState(const State& state)
{
    if (state == _state)
        return;

    _state = state;

    updateState();
}

QString WidgetStateMixin::getTitle() const
{
    return _title;
}