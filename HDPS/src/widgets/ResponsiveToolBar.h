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
#include <QGroupBox>
#include <QDebug>

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
    Q_OBJECT

public:
    PopupPushButton() :
        QPushButton(),
        _popupWidget(nullptr)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        setProperty("cssClass", "popup");
    }

    void setPopupWidget(QWidget* widget) {
        Q_ASSERT(widget != nullptr);

        _popupWidget = widget;

        connect(_popupWidget, &QWidget::windowTitleChanged, [this]() {
            setToolTip(QString("Click to edit %1 settings").arg(_popupWidget->windowTitle().toLower()));
        });

        connect(this, &QPushButton::clicked, [this]() {
            auto popupWidget    = new QWidget(this);
            auto popupLayout    = new QVBoxLayout();
            auto groupBox       = new QGroupBox();
            auto groupBoxLayout = new QVBoxLayout();

            popupWidget->installEventFilter(this);
            popupWidget->setWindowFlags(Qt::Popup);
            popupWidget->setObjectName("PopupWidget");
            popupWidget->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
            popupWidget->setLayout(popupLayout);

            popupLayout->setMargin(7);
            popupLayout->addWidget(groupBox);

            groupBox->setTitle(_popupWidget->windowTitle());
            groupBox->setLayout(groupBoxLayout);

            groupBoxLayout->addWidget(_popupWidget);

            _popupWidget->show();

            popupWidget->move(mapToGlobal(rect().bottomLeft()) - popupWidget->rect().topLeft());
            popupWidget->show();
        });
    }

    bool eventFilter(QObject* object, QEvent* event) {
        switch (event->type())
        {
            case QEvent::FocusOut:
            case QEvent::Close:
                emit popupClosed();
                break;

            default:
                break;
        }

        return QObject::eventFilter(object, event);
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

signals:
    void popupClosed();

protected:
    QWidget*    _popupWidget;
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
    QList<QWidget*> getVisibleWidgets();

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