#pragma once

#include "../util/WidgetResizeEventProxy.h"

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QStackedWidget>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    class Widget : public QStackedWidget
    {
    public:

        enum class State {
            Undefined = -1,
            Popup,
            Compact,
            Full
        };

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

        void setWidgetLayout(QLayout* layout);

    protected:
        std::int32_t            _priority;
        State                   _state;
        QString                 _title;
        QMap<State, QSize>      _sizes;
        QPushButton*            _popupPushButton;
        QWidget*                _widget;

    public:
        const std::int32_t  LAYOUT_MARGIN = 0;
        const std::int32_t  LAYOUT_SPACING = 6;
    };

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    void addWidget(Widget* widget);

private:
    QHBoxLayout*                            _layout;                /** TODO */
    hdps::util::WidgetResizeEventProxy      _widgetEventProxy;      /** TODO */
    QList<Widget*>                          _widgets;               /** TODO */
};

}
}