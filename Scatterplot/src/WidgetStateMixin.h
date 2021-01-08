#pragma once

class WidgetStateMixin
{
public:

    enum class State {
        Popup,
        Compact,
        Full
    };

public:
    WidgetStateMixin(const State& state);

protected:
    State       _state;
};