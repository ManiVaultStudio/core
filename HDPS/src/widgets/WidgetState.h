#pragma once

#include <QWidget>
#include <QMap>

namespace hdps {

namespace gui {

class WidgetState : public QObject {

    Q_OBJECT

public:
    enum class State {
        Undefined = -1,
        Popup,
        Compact,
        Full
    };

    static QMap<QString, State> const states;

    static QString getStateName(const State& state) {
        return states.key(state);
    }

    static State getStateEnum(const QString& stateName) {
        return states[stateName];
    }

public:
    WidgetState(QWidget* widget);

    bool eventFilter(QObject* object, QEvent* event);
    State getState() const;
    void setSize(const State& state, const QSize& size);
    void initialize();
    void updateStateSize(const State& state);
    void changeState(const State& state);

signals:
    void updateState(const State& state);

protected:
    QWidget*        _widget;
    QList<QSize>    _sizes;
};

}
}