#pragma once

#include <QWidget>
#include <QObject>
#include <QString>
#include <QMap>
#include <QSize>
#include <QPushButton>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVariant>
#include <QEvent>

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

    State getState() const {
        const auto stateProperty = parent()->property("state");
        return static_cast<State>(stateProperty.toInt());
    }

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

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT
public:
    

public:

    class Widget : public QStackedWidget
    {
    public:
        Widget(const QString& title, const std::int32_t& priority = 0);

    public:

        QSize sizeHint() const override {
            return currentWidget()->sizeHint();
        }

        QSize minimumSizeHint() const override {
            return currentWidget()->minimumSizeHint();
        }

    public: // State management

        void setState(const WidgetState::State& state, const bool& update = true);

        virtual void updateState() = 0;

    protected:

        void computeStateSizes();

    public:

        std::int32_t getPriority() const;
        void setPriority(const std::int32_t& priority);

        QString getTitle() const;

        QSize getSize(const WidgetState::State& state) const {
            return _sizes[state];
        };

        std::int32_t getWidth(const WidgetState::State& state) const {
            return getSize(state).width();
        };

        void setWidgetLayout(QLayout* layout);

    protected:
        std::int32_t                _priority;
        WidgetState::State                 _state;
        QString                     _title;
        QMap<WidgetState::State, QSize>    _sizes;
        QPushButton*                _popupPushButton;
        QWidget*                    _widget;

    public:
        static const std::int32_t LAYOUT_MARGIN;
        static const std::int32_t LAYOUT_SPACING;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* target, QEvent* event);

    void addWidget(QWidget* widget);

private:
    void updateLayout(QWidget* widget = nullptr);

private:
    QWidget*            _listenWidget;      /** TODO */
    QHBoxLayout*        _layout;            /** TODO */
    QList<QWidget*>     _widgets;           /** TODO */

public:
    static const QSize ICON_SIZE;
};



}
}