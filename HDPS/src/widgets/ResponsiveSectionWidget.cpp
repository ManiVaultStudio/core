#include "ResponsiveSectionWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

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
    _stateSizeHints({{State::Popup, QSize()}, {State::Compact, QSize()}, {State::Full, QSize()}})
{
    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_popupPushButton.get());

    setLayout(_layout);

    _popupPushButton->setIcon(icon);

    _stateWidget = QSharedPointer<QWidget>(getWidgetForState(State::Full));

    // Precompute the initial size hint for each state
    precomputeSizeHintForState(State::Popup);
    precomputeSizeHintForState(State::Compact);
    precomputeSizeHintForState(State::Full);
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
                //computeSizeHints();
                break;

            default:
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

            // Create a new state widget for the state
            _stateWidget = QSharedPointer<QWidget>(getWidgetForState(_state));
            _stateWidget->setWindowOpacity(0);
            _stateWidget->installEventFilter(this);

            // And add the state widget to the layout
            _layout->addWidget(_stateWidget.get());

            // Show the state widget
            _stateWidget->show();

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

QSize ResponsiveSectionWidget::getStateSizeHint(const State& state) const
{
    return _stateSizeHints[state];
}

QSharedPointer<QWidget> ResponsiveSectionWidget::getWidgetForState(const State& state)
{
    auto widget = _getWidgetStateFn(state);

    widget->setWindowTitle(_name);

    if (_initializeWidgetFn)
        _initializeWidgetFn(widget);

    return QSharedPointer<QWidget>(widget);
}

void ResponsiveSectionWidget::precomputeSizeHintForState(const State& state)
{
    switch (state)
    {
        case State::Popup:
            _stateSizeHints[state] = _popupPushButton->sizeHint();
            break;

        case State::Compact:
        case State::Full:
            _stateSizeHints[state] = getWidgetForState(state)->sizeHint();
            break;

        default:
            break;
    }

    qDebug() << QString("%1 initial %2 size hint: [%3,%4] ").arg(_name, getStateName(state), QString::number(_stateSizeHints[state].width()), QString::number(_stateSizeHints[state].height()));
}
}
}