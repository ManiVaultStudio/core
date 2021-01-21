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
#include <QPainter>

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

class PopupPushButton : public QPushButton {
public:

    PopupPushButton() :
        QPushButton()
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        setProperty("cssClass", "popup");
    }

    void paintEvent(QPaintEvent* paintEvent)
    {
        QPushButton::paintEvent(paintEvent);

        QPainter painter(this);
        
        painter.setRenderHint(QPainter::Antialiasing);

        const auto size         = 3.0f;
        const auto bottomRight  = QPointF(rect().bottomRight()) - QPointF(2.0f, 2.0f);
        const auto topLeft      = bottomRight - QPointF(size, size);

        const QVector<QPointF> points{ topLeft, topLeft + QPointF(size, 0.0f), bottomRight - QPointF(size / 2.0f, 0.0f) };

        painter.setPen(Qt::NoPen);
        painter.setBrush(QBrush(isEnabled() ? Qt::black : Qt::gray));
        painter.drawPolygon(points.constData(), points.count());
    }
};

class ResponsiveToolBar : public QWidget
{
    Q_OBJECT

public:
    ResponsiveToolBar(QWidget* parent = nullptr);

    void setListenWidget(QWidget* listenWidget);

    bool eventFilter(QObject* target, QEvent* event);

    void addWidget(QWidget* widget);
    void addStretch(const std::int32_t& stretch = 0);

private:
    void updateLayout(QWidget* widget = nullptr);

private:
    QWidget*            _listenWidget;      /** TODO */
    QHBoxLayout*        _layout;            /** TODO */
    QList<QWidget*>     _widgets;           /** TODO */

public:
    static const std::int32_t LAYOUT_MARGIN;
    static const std::int32_t LAYOUT_SPACING;
    static const QSize ICON_SIZE;
};

}
}