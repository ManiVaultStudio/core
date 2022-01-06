#include "ToolbarSpacerWidget.h"
#include "ToolbarWidget.h"
#include "WidgetAction.h"

#include <QResizeEvent>

namespace hdps {

namespace gui {

ToolbarSpacerWidget::ToolbarSpacerWidget(ToolbarWidget* toolbarWidget) :
    ToolbarItemWidget(toolbarWidget),
    _toolbarWidget(toolbarWidget),
    _layout(),
    _verticalLine(),
    _verticalLineFade(toolbarWidget, &_verticalLine),
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
    //qDebug() << __FUNCTION__ << previousState << currentState;

    const auto sizeBegin = getSize(previousState == State::Undefined ? currentState : previousState);
    const auto sizeEnd = getSize(currentState);
    const auto stateChanged = currentState != previousState;
    const auto widthChanged = sizeBegin != sizeEnd;
    const auto animationDuration = (currentState == State::Undefined || !_toolbarWidget->getEnableAnimation()) ? 0 : (widthChanged ? ToolbarWidget::ANIMATION_DURATION : 0);

    // Width interpolation
    const auto widthLerp = [sizeBegin, sizeEnd](float norm) {
        return sizeBegin.width() + norm * (sizeEnd.width() - sizeBegin.width());
    };

    // Height interpolation
    const auto heightLerp = [sizeBegin, sizeEnd](float norm) {
        return sizeBegin.height() + norm * (sizeEnd.height() - sizeBegin.height());
    };

    // Configure and possibly animate widgets based on the state
    switch (currentState)
    {
        case State::Divider:
        case State::Spacer:
        {
            if (stateChanged)
                _verticalLineFade.fadeIn(animationDuration, animationDuration);

            break;
        }

        case State::Hidden:
        {
            if (stateChanged)
                _verticalLineFade.fadeOut(animationDuration, animationDuration);

            break;
        }
    }

    if (_toolbarWidget->getOrientation() == Qt::Horizontal) {
        /*
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
            setFixedWidth(sizeEnd);
        }
        */
    }
    else {
        /**/
        if (_toolbarWidget->getEnableAnimation()) {
            // Update the widget fixed width when the size animation values changes
            connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp, heightLerp](const QVariant& value) {
                setFixedWidth(widthLerp(value.toFloat()));
                setFixedHeight(heightLerp(value.toFloat()));
            });

            // Set size animation duration and start
            _sizeAnimation.setDuration(animationDuration);
            _sizeAnimation.start();
        }
        else {
            setFixedSize(sizeEnd);
        }

    }
}

QSize ToolbarSpacerWidget::getSize() const
{
    return getSize(_state);
}

QSize ToolbarSpacerWidget::getSize(const std::int32_t& state) const
{
    return QSize(0, 0);
}

ToolbarSpacerWidget::State ToolbarSpacerWidget::getState(const Qt::Orientation& orientation, const ToolbarActionWidget::State& statePrevious, const ToolbarActionWidget::State& stateCurrent)
{
    switch (orientation)
    {
        case Qt::Horizontal:
            break;

        case Qt::Vertical:
        {
            if (statePrevious == ToolbarActionWidget::State::Collapsed && stateCurrent == ToolbarActionWidget::State::Collapsed)
                return ToolbarSpacerWidget::State::Undefined;

            if (statePrevious == ToolbarActionWidget::State::Hidden && stateCurrent == ToolbarActionWidget::State::Hidden)
                return ToolbarSpacerWidget::State::Hidden;

            break;
        }
    }

    return ToolbarSpacerWidget::State::Undefined;
}

QSize ToolbarSpacerWidget::getSize(const Qt::Orientation& orientation, const ToolbarActionWidget::State& statePrevious, const ToolbarActionWidget::State& stateCurrent)
{
    return getSize(orientation, getState(orientation, statePrevious, stateCurrent));
}

QSize ToolbarSpacerWidget::getSize(const Qt::Orientation& orientation, const State& state)
{
    switch (state)
    {
        case Undefined:
            return QSize(0, 0);

        case Divider:
        {
            switch (orientation)
            {
                case Qt::Horizontal:
                    return QSize(0, 0);

                case Qt::Vertical:
                    return QSize(0, 0);
            }

            break;
        }

        case Spacer:
        {
            switch (orientation)
            {
                case Qt::Horizontal:
                    return QSize(ToolbarWidget::ITEM_SPACING, 0);

                case Qt::Vertical:
                    return QSize(0, ToolbarWidget::ITEM_SPACING);
            }

            break;
        }

        case Hidden:
            return QSize(0, 0);

        default:
            break;
    }

    return QSize(0, 0);
}

}
}
