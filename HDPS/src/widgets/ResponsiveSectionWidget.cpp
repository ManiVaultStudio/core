#include "ResponsiveSectionWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>

namespace hdps {

namespace gui {

const QMap<QString, ResponsiveSectionWidget::State> ResponsiveSectionWidget::states = {
    { "Undefined", ResponsiveSectionWidget::State::Undefined },
    { "Popup", ResponsiveSectionWidget::State::Popup },
    { "Compact", ResponsiveSectionWidget::State::Compact },
    { "Full", ResponsiveSectionWidget::State::Full },
    };

ResponsiveSectionWidget::ResponsiveSectionWidget(GetWidgetForStateFn getWidgetStateFn, InitializeWidgetFn initializeWidgetFn, const QString& name, const QIcon& icon /*= QIcon()*/, const std::int32_t& priority /*= 0*/) :
    _getWidgetStateFn(getWidgetStateFn),
    _initializeWidgetFn(initializeWidgetFn),
    _state(State::Undefined),
    _name(name),
    _priority(priority),
    _layout(new QHBoxLayout()),
    _popupPushButton(QSharedPointer<PopupPushButton>::create()),
    _popupWidget(),
    _stateWidget(),
    _sizeHints({{State::Popup, QSize()}, {State::Compact, QSize()}, {State::Full, QSize()}})
{
    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_popupPushButton.get());

    setLayout(_layout);

    _popupPushButton->setIcon(icon);

    _stateWidget = QSharedPointer<QWidget>(getWidgetForState(State::Full));

    computeSizeHints();
}

bool ResponsiveSectionWidget::eventFilter(QObject* object, QEvent* event)
{
    auto widget = dynamic_cast<QWidget*>(object);

    if (widget == _stateWidget) {
        switch (event->type())
        {
            case QEvent::EnabledChange:
                _popupPushButton->setEnabled(_stateWidget->isEnabled());
                break;

            case QEvent::Resize:
                qDebug() << _name << "resized";
                computeSizeHints();
                break;
        }
    }

    return QObject::eventFilter(object, event);
}

QString ResponsiveSectionWidget::getName() const
{
    return _name;
}

hdps::gui::ResponsiveSectionWidget::State ResponsiveSectionWidget::getState() const
{
    return _state;
}

void ResponsiveSectionWidget::setState(const State& state)
{
    if (state == _state)
        return;

    _state = state;

    switch (_state)
    {
        case State::Popup:
        {
            // Create temporary popup widget, assign it to the popup push button and show the popup push button
            _popupWidget = QSharedPointer<QWidget>(getWidgetForState(State::Popup));
            _popupPushButton->setWidget(_popupWidget.get());
            _popupPushButton->show();

            // In popup mode, only the popup push button is visible, so hide the state widget
            _stateWidget->hide();

            break;
        }

        case State::Compact:
        case State::Full:
        {
            // Remove the temporary popup widget and hide the popup push button
            _popupWidget.reset();
            _popupPushButton->hide();

            // Remove event filter (if any)
            if (!_stateWidget.isNull())
                _stateWidget->removeEventFilter(this);

            qDebug() << "A";

            // Create a new state widget for the state
            _stateWidget = QSharedPointer<QWidget>(getWidgetForState(_state));
            _stateWidget->setWindowOpacity(0);
            _stateWidget->installEventFilter(this);

            qDebug() << "B";

            // And add the state widget to the layout
            _layout->addWidget(_stateWidget.get());

            // Show the state widget
            _stateWidget->show();

            qDebug() << "C";

            break;
        }

        default:
            break;
    }

    //qDebug() << QString("%1 state changed to:").arg(_name.toLower()) << static_cast<std::int32_t>(_state);
}

std::int32_t ResponsiveSectionWidget::getPriority() const
{
    return _priority;
}

void ResponsiveSectionWidget::setPriority(const std::int32_t& priority)
{
    if (priority == _priority)
        return;

    _priority = priority;
}

QMap<hdps::gui::ResponsiveSectionWidget::State, QSize> ResponsiveSectionWidget::getSizeHints() const
{
    return _sizeHints;
}

QSize ResponsiveSectionWidget::getSizeHintForState(const State& state) const
{
    return _sizeHints[state];
}

QSize ResponsiveSectionWidget::getSizeHintForState(const std::int32_t& state) const
{
    return getSizeHintForState(static_cast<State>(state));
}

QSharedPointer<QWidget> ResponsiveSectionWidget::getWidgetForState(const State& state)
{
    auto widget = _getWidgetStateFn(state);

    widget->setWindowTitle(_name);

    if (_initializeWidgetFn)
        _initializeWidgetFn(widget);

    return QSharedPointer<QWidget>(widget);
}

void ResponsiveSectionWidget::computeSizeHintForState(const State& state)
{
    switch (state)
    {
        case State::Popup:
            _sizeHints[state] = _popupPushButton->sizeHint();
            break;

        case State::Compact:
        case State::Full:
            _sizeHints[state] = getWidgetForState(state)->sizeHint();
            break;

        default:
            break;
    }

    //qDebug() << QString("%1 initial %2 size hint: [%3,%4] ").arg(_name, getStateName(state), QString::number(_sizeHints[state].width()), QString::number(_sizeHints[state].height()));
}

void ResponsiveSectionWidget::computeSizeHints()
{
    computeSizeHintForState(State::Popup);
    computeSizeHintForState(State::Compact);
    computeSizeHintForState(State::Full);
}

}
}