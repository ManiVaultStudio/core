#include "ResponsiveSectionWidget.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

namespace hdps {

namespace gui {

ResponsiveSectionWidget::ResponsiveSectionWidget(GetWidgetForStateFn getWidgetStateFn, const QString& name, const QIcon& icon /*= QIcon()*/, const std::int32_t& priority /*= 0*/) :
    _getWidgetStateFn(getWidgetStateFn),
    _initializeWidgetFn(),
    _state(State::Undefined),
    _name(name),
    _priority(priority),
    _layout(new QHBoxLayout()),
    _popupPushButton(QSharedPointer<PopupPushButton>::create()),
    _stateWidget(nullptr),
    _stateSizeHints({ QSize(), QSize() , QSize() })
{
    _popupPushButton->setIcon(icon);

    hideWidget(_popupPushButton.get());

    _layout->setMargin(0);
    _layout->setSpacing(0);
    _layout->addWidget(_popupPushButton.get());

    setLayout(_layout);

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
                computeSizeHints();
                break;

            default:
                break;
        }
    }

    return QObject::eventFilter(object, event);
}

void ResponsiveSectionWidget::setInitializeWidgetFunction(InitializeWidgetFn initializeWidgetFn)
{
    _initializeWidgetFn = initializeWidgetFn;
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
            hideWidget(_stateWidget.get(), false);
            showWidget(_popupPushButton.get(), false);
            setStateWidget(getWidget(_state));
            _popupPushButton->setWidget(_stateWidget.get());

            break;
        }

        case State::Compact:
        case State::Full:
        {
            hideWidget(_popupPushButton.get(), false);
            setStateWidget(getWidget(_state));
            _layout->addWidget(_stateWidget.get());
            showWidget(_stateWidget.get(), false);
            break;
        }

        default:
            break;
    }

    //qDebug() << QString("%1 state changed to:").arg(_name.toLower()) << static_cast<std::int32_t>(_state);
}

void ResponsiveSectionWidget::showWidget(QWidget* widget, const bool& animate /*= false*/)
{
    if (!widget)
        return;

    widget->show();
}

void ResponsiveSectionWidget::hideWidget(QWidget* widget, const bool& animate /*= false*/)
{
    if (!widget)
        return;

    widget->hide();
}

void ResponsiveSectionWidget::setStateWidget(QWidget* widget /*= nullptr*/)
{
    if (widget == nullptr)
        return;

    _stateWidget = QSharedPointer<QWidget>(widget);

    _stateWidget->setWindowOpacity(0);
    _stateWidget->installEventFilter(this);
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
    return _stateSizeHints[static_cast<std::int32_t>(state)];
}

QWidget* ResponsiveSectionWidget::getWidget(const State& state)
{
    auto widget = _getWidgetStateFn(state);

    widget->setWindowTitle(_name);

    if (_initializeWidgetFn)
        _initializeWidgetFn(widget);

    return widget;
}

QWidget* ResponsiveSectionWidget::getWidget()
{
    return _stateWidget.get();
}

void ResponsiveSectionWidget::computeSizeHints()
{
    _stateSizeHints[static_cast<std::int32_t>(State::Popup)] = computeStateSizeHint(State::Popup);
    _stateSizeHints[static_cast<std::int32_t>(State::Compact)] = computeStateSizeHint(State::Compact);
    _stateSizeHints[static_cast<std::int32_t>(State::Full)] = computeStateSizeHint(State::Full);

    //qDebug() << QString("Computed %1 size hints:").arg(_name.toLower()) << _stateSizeHints;
}

QSize ResponsiveSectionWidget::computeStateSizeHint(const State& state)
{
    if (state == State::Popup)
        return _popupPushButton->sizeHint();

    const auto stateWidget = getWidget(state);
    const auto stateSizeHint = stateWidget->sizeHint();

    delete stateWidget;

    return stateSizeHint;
}
}
}