#pragma once

#include <QString>

class WidgetStateMixin
{
public:

    enum class State {
        Popup,
        Compact,
        Full
    };

public:
    WidgetStateMixin(const State& state, const QString& title);

    QString getTitle() const {
        return _title;
    };

protected:
    State       _state;
    QString     _title;
};