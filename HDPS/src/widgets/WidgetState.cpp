#include "WidgetState.h"

#include <QDebug>
#include <QEvent>

namespace hdps {

namespace gui {

const QMap<QString, WidgetState::State> WidgetState::states = {
    { "Undefined", WidgetState::State::Undefined },
    { "Popup", WidgetState::State::Popup },
    { "Compact", WidgetState::State::Compact },
    { "Full", WidgetState::State::Full }
};

WidgetState::WidgetState(QWidget* widget) :
    QObject(widget),
    _widget(widget),
    _sizes({ QSize(), QSize(), QSize() })
{
    _widget->installEventFilter(this);
}

bool WidgetState::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::DynamicPropertyChange)
    {
        auto propertyChangeEvent = static_cast<QDynamicPropertyChangeEvent*>(event);

        if (propertyChangeEvent) {
            const auto propertyName = QString(propertyChangeEvent->propertyName().data());

            if (propertyName == "state")
                changeState(getState());
        }
    }

    return QObject::eventFilter(object, event);
}

WidgetState::State WidgetState::getState() const
{
    const auto stateProperty = parent()->property("state");
    return static_cast<State>(stateProperty.toInt());
}

void WidgetState::setSize(const State& state, const QSize& size)
{
    _sizes[static_cast<std::int32_t>(state)] = size;
}

void WidgetState::initialize()
{
    Q_ASSERT(_widget != nullptr);

    changeState(State::Popup);
    changeState(State::Compact);
    changeState(State::Full);
}

void WidgetState::updateStateSize(const State& state)
{
    Q_ASSERT(_widget != nullptr);

    _widget->adjustSize();

    _sizes[static_cast<std::int32_t>(state)] = _widget->sizeHint();

    //qDebug() << "Current state:" << WidgetState::getStateName(state) << _sizes[static_cast<std::int32_t>(state)];

    _widget->setProperty("sizes", QVariant::fromValue(_sizes));
}

void WidgetState::changeState(const State& state)
{
    emit updateState(state);
    updateStateSize(state);
}

}
}