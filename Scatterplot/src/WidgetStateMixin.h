#pragma once

#include <QString>
#include <QSize>

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

    virtual State getState(const QSize& sourceWidgetSize) const = 0;
    void setState(const State& state);

protected:

    virtual void updateState() = 0;

public:

    QString getTitle() const;

protected:
    State       _state;
    QString     _title;

public:
    const std::int32_t  LAYOUT_MARGIN   = 0;
    const std::int32_t  LAYOUT_SPACING  = 4;
};