#pragma once

/*
#include <QStackedWidget>
#include <QString>
#include <QSize>
#include <QMap>
#include <QLayout>

class QPushButton;
class QWidget;

class WidgetStateMixin : public QStackedWidget
{
public:

    enum class State {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

public:
    WidgetStateMixin(const QString& title, const std::int32_t& priority = 0);
    
public:

    QSize sizeHint() const override {
        return currentWidget()->sizeHint();
    }

    QSize minimumSizeHint() const override {
        return currentWidget()->minimumSizeHint();
    }
    
public: // State management

    void setState(const State& state, const bool& update = true);

    virtual void updateState() = 0;

protected:

    void computeStateSizes();

public:

    std::int32_t getPriority() const;
    void setPriority(const std::int32_t& priority);

    QString getTitle() const;

    QSize getSize(const State& state) const {
        return _sizes[state];
    };

    std::int32_t getWidth(const State& state) const {
        return getSize(state).width();
    };

    bool canCollapse() const {
        if (_state == State::Undefined)
            return false;

        return static_cast<std::int32_t>(_state) > static_cast<std::int32_t>(State::Popup);
    }

    bool canExpand() const {
        if (_state == State::Undefined)
            return false;

        return static_cast<std::int32_t>(_state) < static_cast<std::int32_t>(State::Full);
    }

    void setWidgetLayout(QLayout* layout);;

protected:
    std::int32_t            _priority;
    State                   _state;
    QString                 _title;
    QMap<State, QSize>      _sizes;
    QPushButton*            _popupPushButton;
    QWidget*                _widget;

public:
    const std::int32_t  LAYOUT_MARGIN   = 0;
    const std::int32_t  LAYOUT_SPACING  = 4;
};
*/