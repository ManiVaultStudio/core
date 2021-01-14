#pragma once

#include <QString>
#include <QSize>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QIcon>

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
};