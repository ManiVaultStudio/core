#pragma once

#include <QString>

class WidgetStateMixin
{
public:

    enum class State {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

public:
    WidgetStateMixin(const QString& title);
    
public: // State management

    State getState() const {
        return _state;
    }

    void setState(const State& state) {
        if (state == _state)
            return;

        _state = state;

        updateState();
    }

    virtual void updateState() = 0;

public:

    QString getTitle() const {
        return _title;
    }

protected:
    State       _state;
    QString     _title;
};