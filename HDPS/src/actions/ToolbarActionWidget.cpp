#include "ToolbarActionWidget.h"
#include "ToolbarWidget.h"
#include "WidgetAction.h"

#include <QResizeEvent>

namespace hdps {

namespace gui {

ToolbarActionWidget::ToolbarActionWidget(ToolbarWidget* toolbarWidget, WidgetAction& action, std::int32_t priority) :
    ToolbarItemWidget(toolbarWidget),
    _toolbarWidget(toolbarWidget),
    _action(action),
    _priority(priority),
    _layout(),
    _collapsedWidget(action.createCollapsedWidget(this)),
    _standardWidget(_action.createWidget(this)),
    _collapsedWidgetFade(toolbarWidget, _collapsedWidget.get()),
    _standardWidgetFade(toolbarWidget, _standardWidget.get()),
    _sizeAnimation(this)
{
    _state = State::Undefined;

    _collapsedWidgetFade.setOpacity(1.0f);
    _standardWidgetFade.setOpacity(1.0f);

    // Respond to changes in the size of the standard widget
    _standardWidget->installEventFilter(this);

    // Configure size animation
    _sizeAnimation.setDuration(ToolbarWidget::ANIMATION_DURATION);
    _sizeAnimation.setStartValue(0.0f);
    _sizeAnimation.setEndValue(1.0f);
    _sizeAnimation.setEasingCurve(QEasingCurve::InOutQuad);

    _layout.setMargin(0);

    setLayout(&_layout);
}

void ToolbarActionWidget::stateChanged(std::int32_t previousState, std::int32_t currentState)
{
    qDebug() << __FUNCTION__;

    const auto widthBegin           = static_cast<float>(getWidth(previousState == State::Undefined ? currentState : previousState));
    const auto widthEnd             = static_cast<float>(getWidth(currentState));
    const auto stateChanged         = currentState != previousState;
    const auto widthChanged         = widthBegin != widthEnd;
    const auto animationDuration    = (currentState == State::Undefined || !_toolbarWidget->getEnableAnimation()) ? 0 : (widthChanged ? ToolbarWidget::ANIMATION_DURATION : 0);

    // Width interpolation
    const auto widthLerp = [widthBegin, widthEnd](float norm) {
        return widthBegin + norm * (widthEnd - widthBegin);
    };

    // Configure and possibly animate widgets based on the state
    switch (currentState)
    {
        case State::Collapsed:
        {
            // Collapsed widget should respond to mouse events and the standard widget should not
            _collapsedWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _standardWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            // The collapsed widget should be raised above the standard widget
            _collapsedWidget->raise();

            if (stateChanged) {

                // Fade out the standard widget and swap the widgets when done
                _standardWidgetFade.fadeOut(animationDuration, 0, [this]() {
                    _layout.takeAt(0);
                    _layout.addWidget(_collapsedWidget.get());
                });

                // Fade in the collapsed widget when the state changed
                _collapsedWidgetFade.fadeIn(animationDuration, animationDuration);
            }

            break;
        }

        case State::Standard:
        {
            // Collapsed widget should not respond to mouse events and the standard widget should
            _collapsedWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            _standardWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);

            // The standard widget should be raised above the collapsed widget
            _standardWidget->raise();

            if (stateChanged) {

                // Fade in the standard widget when the state changed
                _standardWidgetFade.fadeIn(animationDuration, animationDuration);

                // Fade out the collapsed widget and swap the widgets when done
                _collapsedWidgetFade.fadeOut(animationDuration, 0, [this]() {
                    _layout.takeAt(0);
                    _layout.addWidget(_standardWidget.get());
                });
            }

            break;
        }

        case State::Hidden:
        {
            if (stateChanged) {

                // Fade out the collapsed widget and hide when done
                _collapsedWidgetFade.fadeOut(animationDuration, 0, [this]() {
                    _layout.takeAt(0);
                    _layout.addWidget(_standardWidget.get());
                });
            }

            break;
        }
    }

    if (_toolbarWidget->getEnableAnimation()) {

        // Update the widget fixed width when the size animation values changes
        connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp](const QVariant& value) {
            setFixedWidth(widthLerp(value.toFloat()));
        });

        // Set size animation duration and start
        _sizeAnimation.setDuration(animationDuration);
        _sizeAnimation.start();
    }
    else {
        setFixedWidth(widthEnd);
    }
}

QWidget* ToolbarActionWidget::getWidget(const std::int32_t& state)
{
    switch (static_cast<State>(state))
    {
        case State::Collapsed:
            return _collapsedWidget.get();

        case State::Standard:
            return _standardWidget.get();
    }

    return nullptr;
}

std::int32_t ToolbarActionWidget::getWidth() const
{
    if (_state == State::Hidden)
        return 0;

    return const_cast<ToolbarActionWidget*>(this)->getWidget(static_cast<std::int32_t>(_state))->sizeHint().width();
}

std::int32_t ToolbarActionWidget::getWidth(const std::int32_t& state) const
{
    if (static_cast<State>(state) == State::Hidden)
        return 0;

    return const_cast<ToolbarActionWidget*>(this)->getWidget(static_cast<std::int32_t>(state))->sizeHint().width();
}

std::int32_t ToolbarActionWidget::getPriority() const
{
    return _priority;
}

bool ToolbarActionWidget::eventFilter(QObject* target, QEvent* event)
{
    /*
    switch (event->type())
    {
    case QEvent::Resize:
    {
        if (_state != ItemState::Standard)
            break;

        const auto resizeEvent = static_cast<QResizeEvent*>(event);
        QCoreApplication::processEvents();
        _widget.setFixedWidth(resizeEvent->size().width());
        _toolbarWidget->computeLayout(this);
        break;
    }
    }
    */

    return QObject::eventFilter(target, event);
}

}
}
