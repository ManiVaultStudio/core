#include "ToolbarSpacerWidget.h"
#include "ToolbarWidget.h"
#include "WidgetAction.h"

#include <QResizeEvent>

namespace hdps {

namespace gui {

ToolbarSpacerWidget::ToolbarSpacerWidget(ToolbarWidget* toolbarWidget, WidgetAction& action, std::int32_t priority) :
    ToolbarItemWidget(toolbarWidget),
    _toolbarWidget(toolbarWidget),
    _layout(),
    _verticalLine(),
    _sizeAnimation(this)
{
    _state = State::Undefined;

    _verticalLine.setFrameShape(QFrame::VLine);
    _verticalLine.setFrameShadow(QFrame::Sunken);

    _layout.setMargin(0);
    _layout.setSpacing(0);
    _layout.setAlignment(Qt::AlignCenter);
    _layout.addWidget(&_verticalLine);

    setLayout(&_layout);
}

void ToolbarSpacerWidget::stateChanged(std::int32_t previousState, std::int32_t currentState)
{
    /*
    const auto widthBegin = static_cast<float>(getWidth(_state == ItemState::Undefined ? state : _state));
    const auto widthEnd = static_cast<float>(getWidth(state));
    const auto stateChanged = state != _state;
    const auto widthChanged = widthBegin != widthEnd;
    const auto animationDuration = (_state == ItemState::Undefined || !_toolbarWidget->getEnableAnimation()) ? 0 : (widthChanged ? ANIMATION_DURATION : 0);

    // Width interpolation
    const auto widthLerp = [widthBegin, widthEnd](float norm) {
        return widthBegin + norm * (widthEnd - widthBegin);
    };

    // Configure and possibly animate widgets based on the state
    switch (state)
    {
    case ItemState::Collapsed:
    {
        // Collapsed widget should respond to mouse events and the standard widget should not
        _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
        _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

        // The collapsed widget should be raised above the standard widget
        _collapsedWidget.raise();

        if (stateChanged) {

            // Fade out the standard widget and swap the widgets when done
            _standardWidget.fadeOut(animationDuration, 0, [this]() {
                _widgetLayout.takeAt(0);
                _widgetLayout.addWidget(&_collapsedWidget);
            });

            // Fade in the collapsed widget when the state changed
            _collapsedWidget.fadeIn(animationDuration, animationDuration);
        }

        break;
    }

    case ItemState::Standard:
    {
        // Collapsed widget should not respond to mouse events and the standard widget should
        _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
        _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);

        // The standard widget should be raised above the collapsed widget
        _standardWidget.raise();

        if (stateChanged) {

            // Fade in the standard widget when the state changed
            _standardWidget.fadeIn(animationDuration, animationDuration);

            // Fade out the collapsed widget and swap the widgets when done
            _collapsedWidget.fadeOut(animationDuration, 0, [this]() {
                _widgetLayout.takeAt(0);
                _widgetLayout.addWidget(&_standardWidget);
            });
        }

        break;
    }

    case ItemState::Hidden:
    {
        if (stateChanged) {

            // Fade out the collapsed widget and hide when done
            _collapsedWidget.fadeOut(animationDuration, 0, [this]() {
                _widgetLayout.takeAt(0);
                _widgetLayout.addWidget(&_standardWidget);
            });
        }

        break;
    }
    }

    if (_toolbarWidget->getEnableAnimation()) {

        // Update the widget fixed width when the size animation values changes
        connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp](const QVariant& value) {
            _widget.setFixedWidth(widthLerp(value.toFloat()));
        });

        // Set size animation duration and start
        _sizeAnimation.setDuration(animationDuration);
        _sizeAnimation.start();
    }
    else {
        _widget.setFixedWidth(widthEnd);
    }

    // Assign new state
    _state = state;
    */
}

std::int32_t ToolbarSpacerWidget::getWidth() const
{
    return 0;
}

std::int32_t ToolbarSpacerWidget::getWidth(const std::int32_t& state) const
{
    return 0;
}

}
}
